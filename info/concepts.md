Concepts
========

Nomenclature and key concepts needed for new-coming developers to dive in easier:

 * **surface** - a window (in Wayland language)

 * **sid** - in metanoia every surface have ID, `sid` is a Surface ID

 * **frame** - tree-like structure containing information about position and size of
   surface. Set of all frames is similar concept to i3's tree

 * **selection** - currently selected frame. Selection may include many surfaces.
   It is guaranteed there always exists a selection

 * **anchored frame** - frame which is under automatic management. Metanoia will
   take care about resizing or moving it

 * **floating frame** - any frame which is not anchored

 * **exhibitor** - logical part of program which take care about compositing,
   drawing, frame management, input handling (if you wanted make metanoia an X
   compositor you would probably remove everything except exhibitor)

 * **output** - monitor understood as physical object

 * **display** - monitor understood as abstract concept (e.g. a drawing thread)

 * **workspace** - special frame containing all surfaces exhibited on given
   display. There may be other workspaces on the same displays but will be
   invisible.

 * **root frame** - a single frame containing all known frames

 * **global coordinates** - coordinates describing position of display

 * **workspace coordinates** - coordinates of surfaces within given display
   (workspace)

 * **key modes** - concept similar to vim modes which in basics allows to change
   set of active key bindings.

 * **config** - not mutable user configuration (compiled in or in from text file)

 * **settings** - not mutable information gathered from system (wayland socket,
   environment variables, program arguments)

 * **gears** - mutable user configuration (keymap, key modes, etc.)

Framing
-------

TODO

Threads
-------

Metanoia applied multi-thread approach to deliver best user experience by
handling all events in parallel. To understand how threads are organized we have
to explain couple of terms: **Dispatcher**, **Loops**, **Signals** and **Modules**.

**Dispatcher** is basically a main loop of program. It uses `epoll` mechanism to
listen for all events (keyboard, mouse, monitor plug-in, etc.) in single thread.
This is its main task - wait for events.

Events themselves are handled in Loops. **Loop** is a working thread (or job
queue) processing tasks passed to it. Different kinds of tasks should be
processed in corresponding loops. For example there may be loop handling all
events from devices and other one handling user requests like resizing or moving
window.

Tasks are dispatched to loops via subscribable **Signals** (do not confuse with
Qt signals).  Emitting a signal basically means to create a task which is added
to queue of a loop specified during subscription for that kind of signal. This
provides simple and scalable mechanism which can be used by plugins to react on
events.

Subscription usually should happen at initialization time also processed as a
task. Let's define set of functions realizing certain functionality subscribed
in one initializing task as **Module**.

Let's for example consider handling workspace switch by key combination. At
initialization keyboard module and exhibitor module subscribe for keyboard
events and compositor requests respectively. Dispatcher waits for events from
device, and when they occur it emits specific signals telling about stroke, which
are handled in keyboard module thread. At some point keyboard module notices that
certain key combination occurred which user defined as workspace switch, so it
emits a switch signal. The task is processed in exhibitor module thread because
it was chosen at subscription. Note that keyboard thread and exhibitor thread
could be the same or different threads. This provides us both configurability
and scalability.

---

Now you know theory. For implementation details you can refer to code.

