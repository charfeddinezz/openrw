#include "GangSystem.hpp"

#include <algorithm>

bool GangProfile::addMember(std::uint32_t playerId, const std::string& playerName) {
    if (members.size() >= MaxMembers || hasMember(playerId)) {
        return false;
    }

    members.push_back({playerId, playerName});
    return true;
}

bool GangProfile::removeMember(std::uint32_t playerId) {
    auto found = std::find_if(members.begin(), members.end(),
                              [playerId](const GangMember& member) {
                                  return member.playerId == playerId;
                              });

    if (found == members.end()) {
        return false;
    }

    members.erase(found);
    return true;
}

bool GangProfile::hasMember(std::uint32_t playerId) const {
    auto found = std::find_if(members.begin(), members.end(),
                              [playerId](const GangMember& member) {
                                  return member.playerId == playerId;
                              });

    return found != members.end();
}

bool GangProfile::canJoin(const std::string& password) const {
    return gangPassword.empty() || gangPassword == password;
}

bool GangProfile::setSpawnZone(std::uint32_t zoneId) {
    if (zoneId == 0) {
        return false;
    }

    spawnZoneId = zoneId;
    return true;
}

void GangCommunicationChannel::pushMessage(const std::string& playerName,
                                           const std::string& message) {
    textMessages.push_back(playerName + ": " + message);
}

bool GangCommunicationChannel::canUseVoice() const {
    return voiceState == GangVoiceChannelState::Enabled;
}

bool GangZone::startContest(std::uint32_t attackerGangId) {
    if (attackerGangId == 0 || attackerGangId == ownerGangId ||
        state == GangZoneState::Cooldown) {
        return false;
    }

    contestedByGangId = attackerGangId;
    state = GangZoneState::Contested;
    return true;
}

bool GangZone::completeCapture(std::uint32_t attackerGangId,
                               const std::string& attackerColorHex) {
    if (state != GangZoneState::Contested || attackerGangId == 0 ||
        attackerGangId != contestedByGangId) {
        return false;
    }

    ownerGangId = attackerGangId;
    ownerColorHex = attackerColorHex;
    contestedByGangId = 0;
    cooldownRemainingSeconds = CaptureCooldownSeconds;
    state = GangZoneState::Cooldown;
    return true;
}

void GangZone::tickCooldown(int elapsedSeconds) {
    if (state != GangZoneState::Cooldown || elapsedSeconds <= 0) {
        return;
    }

    cooldownRemainingSeconds = std::max(0, cooldownRemainingSeconds - elapsedSeconds);

    if (cooldownRemainingSeconds == 0) {
        state = GangZoneState::Controlled;
    }
}

bool GangZone::allowsGunfire() const {
    return state != GangZoneState::Cooldown;
}

bool SafeZone::allowsCapture() const {
    return !isHospital;
}

bool SafeZone::allowsGunfire() const {
    return !blocksGunfire;
}

bool GangCombatRules::canDamage(std::uint32_t attackerGangId,
                                std::uint32_t targetGangId) {
    if (attackerGangId == 0 || targetGangId == 0) {
        return true;
    }

    return attackerGangId != targetGangId;
}

bool GangCombatRules::canDamageInSafeZone(std::uint32_t attackerGangId,
                                          std::uint32_t targetGangId,
                                          const SafeZone& safeZone) {
    if (!safeZone.allowsGunfire()) {
        return false;
    }

    return canDamage(attackerGangId, targetGangId);
}
