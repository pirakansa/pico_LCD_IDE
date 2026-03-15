# Documentation Index

This page is the entry point for the repository documentation.

## Reading Guide

| If you want to understand... | Start here |
| --- | --- |
| the overall firmware structure | `module-overview.md` |
| how the firmware boots and enters the runtime loop | `user-guides/startup-flow.md` |
| how to preview the LCD UI on the host | `user-guides/ui-preview.md` |
| how LCD button input is handled | `reference/lcd-input.md` |
| how the event queue behaves | `reference/events-queue.md` |
| how USB HID output is generated | `reference/usb-hid.md` |

## Document Map

```mermaid
flowchart TD
    A[Documentation Index] --> B[Module Overview]
    A --> C[Startup Flow]
    A --> D[UI Preview Guide]
    A --> E[LCD Input Reference]
    A --> F[Event Queue Reference]
    A --> G[USB HID Reference]
    B --> C
    B --> D
    B --> E
    B --> F
    B --> G
    C --> D
    C --> E
    C --> F
    C --> G
    D --> E
    E --> F
    F --> G
```

## Suggested Reading Order

1. Read `module-overview.md` for the architecture.
2. Read `user-guides/startup-flow.md` for control flow.
3. Read `user-guides/ui-preview.md` for host-side UI rendering.
4. Read the reference documents for subsystem details.
