# Event Queue Reference

This reference documents the shared event queue implemented in `src/events/events.c`.

## Scope

The queue is the handoff mechanism between event producers such as the LCD input path and consumers such as the USB HID runtime.

## Public Interface

Declared in `src/events/events.h`:

- `int initialize_ev_data();`
- `stackevents_dt enqueue(stackevents_dt enq_data);`
- `stackevents_dt dequeue();`

The event type is the `stackevents_dt` enum.

## Event Type Definition

Current enum members:

- `STACKEVENTS_NONE`
- `STACKEVENTS_FULL`
- `STACKEVENTS_INTERRUPT`
- `STACKEVENTS_BTN1`
- `STACKEVENTS_BTN2`
- `STACKEVENTS_BTN3`
- `STACKEVENTS_BTN4`

Semantics:

- `STACKEVENTS_NONE`: no queued event is available
- `STACKEVENTS_FULL`: enqueue failed because the queue is full
- `STACKEVENTS_INTERRUPT`: interrupt-style signal used by higher layers
- `STACKEVENTS_BTN1` to `STACKEVENTS_BTN4`: button-originated action events

## Storage Model

The queue is implemented as a fixed-size circular buffer.

Current internal state:

- storage array: `stackevents[QUEUE_SIZE]`
- head index: `queue_head`
- tail index: `queue_tail`
- item count: `queue_cnt`

The queue capacity is currently fixed by:

- `#define QUEUE_SIZE 10`

## Synchronization Model

The queue is protected by `events_queue_mutex`.

Behavior by build mode:

- firmware builds use Pico mutex support through `pico/mutex.h`
- `HOST_TEST` builds use no-op stub mutex functions so host-side tests can compile and run without Pico SDK mutex support

The public queue operations enter the mutex before touching queue state and exit the mutex before returning.

## Initialization Contract

`initialize_ev_data()` resets the queue to an empty state by setting:

- `queue_head = 0`
- `queue_tail = 0`
- `queue_cnt = 0`

Return value:

- always `0` in the current implementation

This function should be called before any producer or consumer uses the queue.

## Enqueue Behavior

`enqueue(stackevents_dt enq_data)` appends one event at the tail position.

On success:

1. the event is written to `stackevents[queue_tail]`
2. `queue_tail` advances modulo `QUEUE_SIZE`
3. `queue_cnt` increments by one
4. the function returns the inserted event value

On overflow:

- the queue is left unchanged
- `Error: Queue overflow.` is printed
- the function returns `STACKEVENTS_FULL`

Overflow condition:

- `QUEUE_SIZE <= queue_cnt`

## Dequeue Behavior

`dequeue()` removes one event from the head position.

On success:

1. the event is read from `stackevents[queue_head]`
2. the slot is reset to `STACKEVENTS_NONE`
3. `queue_head` advances modulo `QUEUE_SIZE`
4. `queue_cnt` decrements by one
5. the removed event is returned

On underflow:

- `Error: Queue underflow.` is printed
- the function returns `STACKEVENTS_NONE`

Underflow condition:

- `queue_cnt <= 0`

## FIFO Semantics

The queue behaves as FIFO storage:

- the first event enqueued is the first event dequeued
- wraparound is handled by modulo arithmetic on the head and tail indices

The current host-side tests cover initialization, FIFO ordering, wraparound, and overflow handling.

## Integration Contract

Expected producer behavior:

- call `initialize_ev_data()` before first use
- treat `STACKEVENTS_FULL` as an enqueue failure

Expected consumer behavior:

- treat `STACKEVENTS_NONE` as "no event available"
- avoid assuming that a returned event is always user input, since higher layers may also use `STACKEVENTS_INTERRUPT`

## Constraints and Current Limitations

- Capacity is fixed at 10 events.
- There is no blocking wait API.
- There is no priority mechanism.
- Underflow and overflow are reported through `printf`.
- The queue stores only `stackevents_dt` values, with no payload beyond the enum value itself.

## Related Documents

- `docs/module-overview.md`
- `docs/user-guides/startup-flow.md`
- `docs/reference/lcd-input.md`
- `docs/reference/usb-hid.md`