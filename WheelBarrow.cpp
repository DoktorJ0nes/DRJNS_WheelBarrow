#include <windows.h>
#include <stdio.h>
#include <psapi.h>
#include <string>
#include <vector>
#include <fstream>
#include <unordered_set>


// Each entry in a PatchGroup is one byte replacement at a signed offset from the pattern match.
struct PatchOp {
    INT64 patchOffset;          // Offset from pattern match to patch location
    const BYTE* bytes;          // Bytes to write
    SIZE_T size;                // Patch size
};

// A PatchGroup is one logical, named patch. It locates a single pattern in memory and then applies one or more byte replacements at offsets from that match.
struct PatchGroup {
    const char* identifier;     // Identifier used in config and welcome message
    const BYTE* pattern;        // Bytes to search for
    SIZE_T patternSize;         // Pattern size
    const PatchOp* ops;         // Array of byte replacements
    SIZE_T opCount;             // Number of byte replacements
};

// --- DLL module handle (used to locate the config file next to the DLL) ---
static HMODULE g_hModule = nullptr;

// --- Pattern Scanner ---
BYTE* FindPattern(BYTE* base, SIZE_T moduleSize, const BYTE* pattern, SIZE_T patternSize) {
    for (SIZE_T i = 0; i + patternSize <= moduleSize; i++) {
        bool found = true;
        for (SIZE_T j = 0; j < patternSize; j++) {
            if (base[i + j] != pattern[j]) {
                found = false;
                break;
            }
        }
        if (found) return base + i;
    }
    return nullptr;
}

// --- Patch definitions ---

// extend persistence range for vehicles and trailers to full map
const BYTE PATTERN1[] = { 0x0F, 0x28, 0xCB, 0x0F, 0x28, 0xC5, 0x0F, 0xC6, 0xC5, 0xFF, 0xF3, 0x0F, 0x58, 0xC8, 0x0F, 0x28, 0xC5, 0x0F, 0xC6, 0xC5, 0xAA, 0xF3, 0x0F, 0x5C, 0xC2, 0xF3, 0x0F, 0x5C, 0xEB };

const BYTE PATCH1A_BYTES[] = { 0xF3, 0x0F, 0x58, 0xF6 }; // override MULSS XMM2, XMM6 with ADDSS XMM6, XMM6 (24.0 + 24.0 = 48.0)

const BYTE PATCH1B_BYTES[] = { 0xF3, 0x0F, 0x59, 0xF6,   // override MULSS XMM3, XMM6 with MULSS XMM6, XMM6 (48.0 * 48.0 = 2304.0)
							  0x0F, 0x28, 0xE6};         // override MOVAPS XMM4, XMM2 with MOVAPS XMM4, XMM6

const BYTE PATCH1C_BYTES[] = { 0x0F, 0x28, 0xCE,         // override MOVAPS XMM1, XMM3 with MOVAPS XMM1, XMM6
							  0x0F, 0x28, 0xC5,          // original MOVAPS XMM0, XMM5
							  0x0F, 0xC6, 0xC5, 0xFF,    // original SHUFPS XMM0, XMM5, 0xFF
							  0xF3, 0x0F, 0x58, 0xC8,    // original ADDSS XMM1, XMM0
							  0x0F, 0x28, 0xC5,          // original MOVAPS XMM0, XMM5
							  0x0F, 0xC6, 0xC5, 0xAA,    // original SHUFPS XMM0, XMM5, 0xAA
							  0xF3, 0x0F, 0x5C, 0xC6,    // override SUBSS XMM0, XMM2 with SUBSS XMM0, XMM6
							  0xF3, 0x0F, 0x5C, 0xEE     // override SUBSS XMM5, XMM3 with SUBSS XMM5, XMM6
							  };

// disbale CRC check for map to prevent mudmaps and plants to reset when map was updated
const BYTE PATTERN2[] = { 0x41, 0x0F, 0x95, 0xC7, 0x84, 0xDB, 0x74, 0x05, 0x41, 0x3B, 0xCD, 0x75, 0x06 };
const BYTE PATCH2_BYTES[] = { 0x90, 0x90, 0x90, 0x90 }; // override SETNZ R15B with NOP NOP NOP NOP

// force loading user mud maps on older savegame versions
const BYTE PATTERN3[] = { 0x74, 0x79, 0x8b, 0x45, 0x90, 0x3b, 0xc6, 0x0f, 0x4d, 0xc6, 0x89, 0x45, 0x90, 0x89, 0x45, 0xe0, 0x8b, 0x45, 0xa0, 0x3b, 0xc6 };
const BYTE PATCH3_BYTES[] = { 0x90, 0x90 };

// make grass for non-mud terrain-blocks persistent
const BYTE PATTERN4[] = { 0x4d, 0x8b, 0x46, 0x48, 0x48, 0x8b, 0xd6, 0x49, 0x8b, 0xcd, 0xe8, 0x28, 0x6a, 0x01, 0x00, 0x48, 0x8b, 0x4e, 0x28, 0x49, 0x8b, 0x5e, 0x48, 0x48, 0x85, 0xc9, 0x74, 0x06, 0x48, 0x8b, 0x01, 0xff, 0x50, 0x08 };
const BYTE PATCH4_BYTES[] = { 0x49, 0x8b, 0x5e, 0x48, 0x48, 0x8b, 0x4e, 0x28, 0x48, 0x85, 0xc9, 0x74, 0x06, 0x48, 0x8b, 0x01, 0xff, 0x50, 0x08, 0x48, 0x89, 0x5e, 0x28, 0x41, 0x8b, 0xc4, 0xf0, 0x0f, 0xc1, 0x43, 0x08, 0x4c, 0x8b, 0xc3, 0x48, 0x8b, 0xd6, 0x49, 0x8b, 0xcd, 0xe8, 0x0a, 0x6a, 0x01, 0x00, 0x90 };

// --- Patch operations grouped per logical patch ---

const PatchOp PATCH1_OPS[] = {
    { -27, PATCH1A_BYTES, sizeof(PATCH1A_BYTES) },
    { -15, PATCH1B_BYTES, sizeof(PATCH1B_BYTES) },
    {   0, PATCH1C_BYTES, sizeof(PATCH1C_BYTES) },
};

const PatchOp PATCH2_OPS[] = {
    { 0, PATCH2_BYTES, sizeof(PATCH2_BYTES) },
};

const PatchOp PATCH3_OPS[] = {
    { 0, PATCH3_BYTES, sizeof(PATCH3_BYTES) },
};

const PatchOp PATCH4_OPS[] = {
    { 0, PATCH4_BYTES, sizeof(PATCH4_BYTES) },
};

// --- Patch group list ---
// To add a new patch: define a PATTERN, the bytes to write, a matching PatchOp array, then add a PatchGroup entry below with a unique identifier string.
const PatchGroup PATCH_GROUPS[] = {
    { "extended_persistence_range", PATTERN1, sizeof(PATTERN1), PATCH1_OPS, sizeof(PATCH1_OPS) / sizeof(PATCH1_OPS[0]) },
    { "map_crc_check_skip",         PATTERN2, sizeof(PATTERN2), PATCH2_OPS, sizeof(PATCH2_OPS) / sizeof(PATCH2_OPS[0]) },
	{ "old_savegame_support",       PATTERN3, sizeof(PATTERN3), PATCH3_OPS, sizeof(PATCH3_OPS) / sizeof(PATCH3_OPS[0]) },
    { "grass_persistency",          PATTERN4, sizeof(PATTERN4), PATCH4_OPS, sizeof(PATCH4_OPS) / sizeof(PATCH4_OPS[0]) },
};

// --- Config loader ---
// The config file lives next to the DLL and is never written by the program; it is maintained manually by the user. If the file is missing, no patches are disabled and the welcome message is shown - this is the default behavior.

std::string GetConfigPath() {
    char path[MAX_PATH] = { 0 };
    if (GetModuleFileNameA(g_hModule, path, MAX_PATH) == 0) {
        return std::string();
    }
    std::string p(path);
    size_t pos = p.find_last_of("\\/");
    if (pos != std::string::npos) {
        p = p.substr(0, pos + 1);
    } else {
        p.clear();
    }
    return p + "WheelBarrow.cfg";
}

std::unordered_set<std::string> LoadDisabledItems() {
    std::unordered_set<std::string> disabled;
    std::string cfgPath = GetConfigPath();
    if (cfgPath.empty()) return disabled;

    std::ifstream file(cfgPath);
    if (!file.is_open()) return disabled; // No config file -> nothing disabled

    std::string line;
    while (std::getline(file, line)) {
        // Strip everything from '#' onward (line comments and inline comments)
        size_t hashPos = line.find('#');
        if (hashPos != std::string::npos) {
            line = line.substr(0, hashPos);
        }
        // Trim leading and trailing whitespace
        size_t start = line.find_first_not_of(" \t\r\n");
        if (start == std::string::npos) continue;
        size_t end = line.find_last_not_of(" \t\r\n");
        line = line.substr(start, end - start + 1);

        if (!line.empty()) disabled.insert(line);
    }
    return disabled;
}

// --- Patch application ---
// Returns true if all ops within the group were applied successfully. The pattern is searched once per group; all ops then patch at offsets from that single match.

bool ApplyPatchGroup(const PatchGroup& group, BYTE* base, SIZE_T moduleSize) {
    BYTE* match = FindPattern(base, moduleSize, group.pattern, group.patternSize);
    if (!match) return false;

    for (SIZE_T i = 0; i < group.opCount; i++) {
        const PatchOp& op = group.ops[i];
        BYTE* target = match + op.patchOffset;

        DWORD oldProtect;
        if (!VirtualProtect(target, op.size, PAGE_EXECUTE_READWRITE, &oldProtect)) {
            return false;
        }
        memcpy(target, op.bytes, op.size);
        VirtualProtect(target, op.size, oldProtect, &oldProtect);
        FlushInstructionCache(GetCurrentProcess(), target, op.size);
    }
    return true;
}

void ApplyAllPatches() {
    BYTE* base = (BYTE*)GetModuleHandleW(L"SnowRunner.exe");
    if (!base) {
        MessageBoxA(NULL, "Applying patches failed!\nSnowRunner.exe is not running.", "WheelBarrow", MB_OK | MB_ICONERROR);
        return;
    }

    MODULEINFO mi{};
    GetModuleInformation(GetCurrentProcess(), (HMODULE)base, &mi, sizeof(mi));

    std::unordered_set<std::string> disabled = LoadDisabledItems();
    bool showWelcome = disabled.count("wheelbarrow_welcome_message") == 0;

    std::vector<std::string> applied;
    std::vector<std::string> failed;
    std::vector<std::string> skipped;

    for (SIZE_T i = 0; i < sizeof(PATCH_GROUPS) / sizeof(PATCH_GROUPS[0]); i++) {
        const PatchGroup& group = PATCH_GROUPS[i];
        if (disabled.count(group.identifier)) {
            skipped.push_back(group.identifier);
            continue;
        }
        if (ApplyPatchGroup(group, base, mi.SizeOfImage)) {
            applied.push_back(group.identifier);
        } else {
            failed.push_back(group.identifier);
        }
    }

    // Errors are always reported, even when the welcome message is disabled in the config.
    if (!showWelcome && failed.empty()) return;

    std::string msg;

    if (!applied.empty()) {
        msg += "Applied patches:\n";
        for (const std::string& id : applied) msg += "  - " + id + "\n";
		msg += "\n";
    }
    if (!failed.empty()) {
        msg += "Failed patches:\n";
        for (const std::string& id : failed) msg += "  - " + id + "\n";
		msg += "\nYou can still play the game, but things could go wrong!\nBE WARNED!\n\n";
    }
	
    msg += "ALWAYS MAKE BACKUPS!\nYou can disable this message in the WheelBarrow.cfg file.";

    UINT icon = failed.empty() ? MB_ICONINFORMATION : MB_ICONERROR;
    MessageBoxA(NULL, msg.c_str(), "WheelBarrow by Dr_J0nes", MB_OK | icon);
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD reason, LPVOID reserved) {
    if (reason == DLL_PROCESS_ATTACH) {
        g_hModule = hModule;
        DisableThreadLibraryCalls(hModule);
        ApplyAllPatches();
    }
    return TRUE;
}