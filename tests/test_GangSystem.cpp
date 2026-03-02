#include <boost/test/unit_test.hpp>

#include <cstdint>
#include <string>

#include <multiplayer/GangSystem.hpp>

BOOST_AUTO_TEST_SUITE(GangSystemTests)

BOOST_AUTO_TEST_CASE(test_gang_member_limit_and_membership) {
    GangProfile gang;

    for (std::uint32_t id = 1; id <= GangProfile::MaxMembers; ++id) {
        BOOST_CHECK(gang.addMember(id, "player" + std::to_string(id)));
    }

    BOOST_CHECK_EQUAL(gang.members.size(), GangProfile::MaxMembers);
    BOOST_CHECK(!gang.addMember(42, "overflow"));
    BOOST_CHECK(!gang.addMember(1, "duplicate"));
    BOOST_CHECK(gang.hasMember(5));
}

BOOST_AUTO_TEST_CASE(test_remove_member) {
    GangProfile gang;

    BOOST_CHECK(gang.addMember(1, "alpha"));
    BOOST_CHECK(gang.addMember(2, "beta"));
    BOOST_CHECK(gang.removeMember(1));
    BOOST_CHECK(!gang.hasMember(1));
    BOOST_CHECK(!gang.removeMember(77));
}

BOOST_AUTO_TEST_CASE(test_gang_password_and_spawn_zone) {
    GangProfile gang;
    gang.gangPassword = "secret";

    BOOST_CHECK(gang.canJoin("secret"));
    BOOST_CHECK(!gang.canJoin("wrong"));

    BOOST_CHECK(!gang.setSpawnZone(0));
    BOOST_CHECK(gang.setSpawnZone(44));
    BOOST_CHECK_EQUAL(gang.spawnZoneId, 44);
}

BOOST_AUTO_TEST_CASE(test_gang_communication_channel) {
    GangCommunicationChannel channel;
    channel.gangId = 99;

    BOOST_CHECK(channel.canUseVoice());
    channel.pushMessage("alice", "hold the bridge");

    BOOST_CHECK_EQUAL(channel.textMessages.size(), 1);
    BOOST_CHECK_EQUAL(channel.textMessages[0], "alice: hold the bridge");

    channel.voiceState = GangVoiceChannelState::Disabled;
    BOOST_CHECK(!channel.canUseVoice());
}

BOOST_AUTO_TEST_CASE(test_zone_capture_lifecycle) {
    GangZone zone;
    zone.ownerGangId = 10;
    zone.ownerColorHex = "#00AA00";

    BOOST_CHECK(zone.startContest(20));
    BOOST_CHECK(zone.state == GangZoneState::Contested);
    BOOST_CHECK(zone.allowsGunfire());

    BOOST_CHECK(zone.completeCapture(20, "#0000FF"));
    BOOST_CHECK(zone.state == GangZoneState::Cooldown);
    BOOST_CHECK_EQUAL(zone.ownerGangId, 20);
    BOOST_CHECK_EQUAL(zone.cooldownRemainingSeconds,
                      GangZone::CaptureCooldownSeconds);
    BOOST_CHECK(!zone.allowsGunfire());

    zone.tickCooldown(60);
    BOOST_CHECK_EQUAL(zone.cooldownRemainingSeconds,
                      GangZone::CaptureCooldownSeconds - 60);

    zone.tickCooldown(120);
    BOOST_CHECK(zone.state == GangZoneState::Controlled);
    BOOST_CHECK(zone.allowsGunfire());
}

BOOST_AUTO_TEST_CASE(test_safe_zone_rules) {
    SafeZone hospital;
    hospital.isHospital = true;
    hospital.blocksGunfire = true;

    BOOST_CHECK(!hospital.allowsCapture());
    BOOST_CHECK(!hospital.allowsGunfire());
    BOOST_CHECK(!GangCombatRules::canDamageInSafeZone(1, 2, hospital));

    SafeZone combatZone;
    combatZone.isHospital = false;
    combatZone.blocksGunfire = false;

    BOOST_CHECK(combatZone.allowsCapture());
    BOOST_CHECK(combatZone.allowsGunfire());
    BOOST_CHECK(!GangCombatRules::canDamageInSafeZone(2, 2, combatZone));
    BOOST_CHECK(GangCombatRules::canDamageInSafeZone(1, 2, combatZone));
}

BOOST_AUTO_TEST_CASE(test_friendly_fire_blocked_for_same_gang) {
    BOOST_CHECK(GangCombatRules::canDamage(1, 2));
    BOOST_CHECK(!GangCombatRules::canDamage(2, 2));
    BOOST_CHECK(GangCombatRules::canDamage(0, 2));
    BOOST_CHECK(GangCombatRules::canDamage(2, 0));
}

BOOST_AUTO_TEST_SUITE_END()
