#ifndef _RWENGINE_GANGSYSTEM_HPP_
#define _RWENGINE_GANGSYSTEM_HPP_

#include <cstdint>
#include <string>
#include <vector>

struct PlayerAccount {
    std::uint32_t accountId{};
    std::string username{};
    std::string passwordHash{};
};

struct CharacterProfile {
    std::uint32_t characterId{};
    std::uint32_t accountId{};
    std::string displayName{};
    std::string clothingSet{};
};

struct GangMember {
    std::uint32_t playerId{};
    std::string playerName{};
};

struct GangProfile {
    static const std::size_t MaxMembers = 20;

    std::uint32_t gangId{};
    std::string gangName{};
    std::string gangPassword{};
    std::string gangColorHex{"#FFFFFF"};
    std::string gangMotto{};
    std::uint32_t spawnZoneId{};
    std::vector<GangMember> members{};

    bool addMember(std::uint32_t playerId, const std::string& playerName);
    bool removeMember(std::uint32_t playerId);
    bool hasMember(std::uint32_t playerId) const;
    bool canJoin(const std::string& password) const;
    bool setSpawnZone(std::uint32_t zoneId);
};

enum class GangVoiceChannelState {
    Disabled,
    Enabled
};

struct GangCommunicationChannel {
    std::uint32_t gangId{};
    GangVoiceChannelState voiceState{GangVoiceChannelState::Enabled};
    std::vector<std::string> textMessages{};

    void pushMessage(const std::string& playerName, const std::string& message);
    bool canUseVoice() const;
};

enum class GangZoneState {
    Neutral,
    Contested,
    Cooldown,
    Controlled
};

struct GangZone {
    static const int CaptureCooldownSeconds = 180;

    std::uint32_t zoneId{};
    std::string zoneName{};
    std::uint32_t ownerGangId{};
    std::string ownerColorHex{"#FFFFFF"};
    GangZoneState state{GangZoneState::Neutral};

    std::uint32_t contestedByGangId{};
    std::string contestedColorHex{"#FF0000"};
    int cooldownRemainingSeconds{};

    bool startContest(std::uint32_t attackerGangId);
    bool completeCapture(std::uint32_t attackerGangId,
                         const std::string& attackerColorHex);
    void tickCooldown(int elapsedSeconds);
    bool allowsGunfire() const;
};

struct SafeZone {
    std::uint32_t zoneId{};
    std::string zoneName{};
    bool isHospital{false};
    bool blocksGunfire{true};

    bool allowsCapture() const;
    bool allowsGunfire() const;
};

struct GangCombatRules {
    static bool canDamage(std::uint32_t attackerGangId,
                          std::uint32_t targetGangId);
    static bool canDamageInSafeZone(std::uint32_t attackerGangId,
                                    std::uint32_t targetGangId,
                                    const SafeZone& safeZone);
};

#endif
