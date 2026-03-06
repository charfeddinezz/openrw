# Gang Multiplayer MVP Plan

This repository is currently a single-player engine. To support the requested feature set,
this MVP adds **core domain rules** that can be connected later to UI/network/gameplay systems.

## Implemented in this patch

- Gang profile model with:
  - gang id, name, password, color, motto
  - member list with id + name
  - strict 20 member cap
  - configurable gang spawn zone id
- Gang communication model with:
  - per-gang text message stream
  - voice channel enabled/disabled state flag
- Friendly-fire and safe-zone helpers:
  - members in the same gang cannot damage each other
  - hospital zones are non-capturable and can block gunfire
- Gang war zone state machine:
  - neutral/contested/cooldown/controlled
  - contested phase (flash red on map can be driven from this state)
  - 3-minute no-gunfire cooldown after successful capture

## Not yet implemented

- Account login/registration UI flow
- Character dashboard + clothing customization UI flow
- Real networked online mode and server authority
- Voice chat transport (microphone stream codec/network)
- Spawn-point selection and synchronized vehicle seat logic
- Dynamic map rendering and client HUD for territory/safe-zone overlays
- Admin tools for hospital safe-zone circle authoring

These require additional subsystems (auth backend, netcode, voice stack, admin tooling, UI).
