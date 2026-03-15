# Documentation Index

This page is the entry point for the repository documentation.

## Reading Guide

| If you want to understand... | Start here |
| --- | --- |
| the overall firmware structure | `module-overview.md` |
| how the firmware boots and enters the runtime loop | `user-guides/startup-flow.md` |
| how LCD button input is handled | `reference/lcd-input.md` |
| how the event queue behaves | `reference/events-queue.md` |
| how USB HID output is generated | `reference/usb-hid.md` |

## Document Map

```mermaid
flowchart TD
    A[Documentation Index] --> B[Module Overview]
    A --> C[Startup Flow]
    A --> D[LCD Input Reference]
    A --> E[Event Queue Reference]
    A --> F[USB HID Reference]
    B --> C
    B --> D
    B --> E
    B --> F
    C --> D
    C --> E
    C --> F
    D --> E
    E --> F
```

## Suggested Reading Order

1. Read `module-overview.md` for the architecture.
2. Read `user-guides/startup-flow.md` for control flow.
3. Read the reference documents for subsystem details.