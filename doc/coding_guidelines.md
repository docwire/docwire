# DocWire Coding Guidelines

## RULES (Meta & Governance)

### [RULES-NAMES-TRIPARTITE]

**The Rule:** All architectural comments and guideline references MUST use the strict `[CATEGORY-TOPIC-SOLUTION]` uppercase format for global searchability.

**The Why:** Ensures that PR reviews and code comments tie directly back to the architectural foundation without ambiguity.

### [RULES-ORIGIN-MANIFESTO]

**The Rule:** Every technical rule must be directly traceable to the core philosophy of predictability, performance, and transparency.

**The Why:** Rules without a philosophical anchor become subjective red tape. If a rule doesn't serve the Manifesto, it is deleted.

### [RULES-ARCHITECTURE-ABSTRACT]

**The Rule:** Architectural guidelines must strictly define system physics, memory boundaries, and structural policies. They must explicitly avoid hardcoding implementation details, syntactic sugar, internal helper names, or locking the architecture to a specific 3rd-party tool. Where tools or syntax must be mentioned for clarity, they must be strictly contextualized as examples (e.g., "such as...").

**The Why:** Implementations, helper macros, and developer tooling naturally churn over a project's lifecycle. If architectural rules are tied to specific syntax or temporary tools, the document becomes brittle, requires constant maintenance, and quickly loses authority. Abstract, physics-based rules remain timeless.

### [RULES-SNIPPETS-DISTINCTION]

**The Rule:** Code snippets within these guidelines contain two distinct categories of code: **Core Infrastructure** and **Domain Examples**.

1. **Core Infrastructure** (e.g., `docwire::resource_factory`, `docwire::pipeline_step`, `docwire::destination`, `docwire::annotated::object`): These are mandated, foundational SDK templates. Even if they are currently being drafted, they represent strict architectural law. When the rules state you *must* use them, it is a hard requirement.

2. **Domain Examples** (e.g., `pdf::writer`, `zip::stream_deferred`, `fs::file_name`): These are hypothetical illustrations used to demonstrate *how* to apply the Core Infrastructure. They represent the required shape and naming conventions for future code, regardless of whether those specific classes exist in the repository today.

**The Why:** Clearly distinguishing between the mandatory SDK engine components and hypothetical domain implementations ensures developers know exactly which templates they are forced to use, while understanding how to correctly design the new domain modules they are tasked with building.

### [RULES-COMPLIANCE-FORWARD]

**The Rule:** These guidelines represent the architectural goal. All new code must adhere to them, but developers must accept existing non-compliant code. Do not block PRs to fix legacy violations unless explicitly planned.

**The Why:** Prevents scope creep in pull requests and keeps the team shipping features instead of endlessly refactoring legacy code.

### [RULES-ITERATIVE-LIFECYCLE]

**The Rule:** We prioritize business velocity over Day-1 architectural purity. New features and 3rd-party integrations are permitted to temporarily violate strict performance rules (e.g., hidden heap allocations, caught exceptions) provided they are strictly quarantined behind a compliant DocWire API boundary. Optimization and refactoring are driven strictly by profiling data, not theoretical perfection.

**The Why:** Demanding 100% compliance before merging a new feature stalls product delivery. By isolating the messy code early, we ship the feature to users today, and we rely on real-world profiling to tell us if the architectural violation is actually causing a bottleneck worth fixing tomorrow.

### [RULES-DEFAULTS-CPP_CORE]

**The Rule:** For general C++ practices not covered here, the codebase defaults to the C++ Core Guidelines. This document purely defines DocWire-specific overrides.

**The Why:** Prevents this document from bloating into a 100-page generic C++ manual, keeping the focus entirely on high-performance edge architecture.

## DESIGN (Core Architecture & C++)

### [DESIGN-PRIORITY-HIERARCHY]

**The Rule:** Architectural conflicts must be resolved using this strict hierarchy of values: 1. Safety → 2. Predictability → 3. Peak Throughput → 4. Binary Size → 5. Build Performance → 6. ABI/API Stability → 7. Developer Convenience.

**The Why:** DocWire is a mission-critical edge toolkit. We will gladly write boilerplate (#7) or break ABI (#6) to achieve peak throughput (#3). We will sacrifice compile times (#5) to keep binaries small (#4). Above all, we never sacrifice determinism (#2) or safety (#1) for speed.

### [DESIGN-LANG-CPP]

**The Rule:** The SDK must be written entirely in C++.

**The Why:** C++ was chosen for three fundamental architectural strengths:

* **Deterministic Memory Control:** C++ provides the absolute, explicit control over memory layout and object lifecycles required to build our custom O(1) architecture.

* **Zero-Cost Abstractions:** It allows us to build extremely fast, allocation-free data pipelines using high-level concepts (templates, RTTI variants) that compile down to uncompromising, highly optimized machine code.

* **Universal Portability:** C++ makes the SDK universally embeddable. The exact same core parser can run natively on iOS/Android, compile to WebAssembly for the browser, or be wrapped cleanly in Python/Node.js enterprise bindings without heavy runtime overhead.

### [DESIGN-DIALECT-CPP20]

**The Rule:** The codebase must strictly target the C++20 standard dialect. Bumping the compiler requirement to C++23 or newer is strictly prohibited.

**The Why:**

* **The C++20 Sweet Spot:** C++20 provides the exact foundational tools we need for safe pipelines (`std::span` for zero-copy parsing, `consteval` for hardcoded depth limits, and Concepts to enforce static polymorphism across our lazy-evaluation pipelines).

* **Enterprise Compatibility:** Enterprise clients (defense, finance, embedded hardware) upgrade their compiler toolchains conservatively. Freezing the standard at C++20 ensures maximum commercial compatibility. If we need paradigms from newer standards (like C++23's `std::expected`), we implement them internally (`docwire::expected`) so we can deploy them immediately while enforcing our own stricter, exception-free constraints.

### [DESIGN-DISPATCH-STATIC]

**The Rule:** The SDK must strictly ban dynamic dispatch (`virtual` functions, raw function pointers, and type-erased wrappers like `std::function`). Polymorphic components (parsers, exporters) must be composed using Static Polymorphism (C++20 Concepts and Templates). Heterogeneous data must use tagged unions (e.g., `std::variant`).

**The Why:** Virtual functions and type-erased pointers (like `std::function`) force the CPU into indirect branch prediction misses, trigger hidden OS heap allocations, and block the compiler's inlining optimizer. By passing domain objects directly into templated pipelines, the compiler perfectly resolves dependencies via Argument-Dependent Lookup (ADL) at compile time. This natively provides zero-cost lazy evaluation without the need for mutable state or type-erasure.

### [DESIGN-MACRO-BAN]

**The Rule:** `#define` macros are universally banned for logic, constants, and logging. Feature toggles must use `if constexpr`. Constants must use `constexpr` or `consteval`. Short-circuit evaluation must use C++20 Concepts and lambdas. Context capture must use `std::source_location`.

**The Exception:** Macros are strictly quarantined to physical compilation necessities: Header include guards (`#ifndef`) and dynamic library export visibility (`DOCWIRE_API`).

**The Why:** C++20 Modules do not export macros. Relying on macros for API logic breaks our `[BUILD-FUTURE-MODULES]` migration path. Quarantining them to compilation boundaries keeps the C++ namespace mathematically pure while allowing standard DLL compilation.

### [DESIGN-METHODS-STATEONLY]

**The Rule:** Member functions of data structures must only read or mutate their own internal state.

**The Why:** Embedding external operational logic inside a data struct creates tight architectural coupling. Cross-module operations must be decoupled into external pipelines.

### [DESIGN-BOUNDARY-NORMALIZED]

**The Rule:** System boundaries (parsers, deserializers) must act as an Anti-Corruption Layer. They must resolve implicit defaults and normalize chaotic external inputs into a mathematically predictable, strict internal state.

**The Why:** If internal data allows implicit or missing states, downstream algorithms are forced to implement defensive checks. Normalizing at the boundary guarantees deterministic downstream execution and eliminates branch overhead.

### [DESIGN-POLICY-MECHANISM_FIRST]

**The Rule:** The SDK must provide composable mechanisms (allocator-aware components, decoupled parsers) without dictating application policy (memory topologies, threading models). We provide a zero-configuration default, but never lock the architecture.

**The Why:** We do not know the host application's hardware constraints. An embedded edge device may require a single, statically allocated global memory pool, while a high-throughput cloud backend may demand isolated, thread-local allocators for every parsed document. Forcing a specific lifecycle policy locks out enterprise integrations.

### [DESIGN-AUDIT-UNIFIED]

**The Rule:** The SDK must use a single, dependency-injected `Audit` mechanism (passed strictly via the execution context) for recording telemetry and diagnostic history. The `Audit` interface must accept structured, raw C++ values and avoid premature string serialization. **Crucially, the `Audit` object must never be responsible for control-flow, panics, or system termination. Furthermore, static or global stateless `Audit` logging is strictly banned.**

**The Why:** The SDK dictates the *mechanism* (emitting type-safe events); the user's host application dictates the *policy*. By receiving raw C++ objects, the user can use zero-cost compile-time templates to route data to an Arena-backed container, serialize it to a JSON logger, or completely optimize it out of existence in Release builds via Dead Code Elimination. Restricting `Audit` from handling crashes ensures it remains decoupled from the low-level `SafetyPolicy`. Banning static logging ensures that all telemetry remains thread-local and lock-free, honoring the core dependency injection architecture.

### [DESIGN-SAFETY-POLICY_BASED]

**The Rule:** The SDK must enforce internal API preconditions (e.g., array bounds, null-checks) using a stateless template policy class (`SafetyPolicy`), which must default to a strict configuration.

This policy class must contain:

1. **Granular Compile-Time Flags:** `constexpr bool` flags (e.g., `check_bounds`, `check_nulls`) that dictate whether internal validation blocks are compiled or completely erased.

2. **A Stateless Panic Hook:** A `[[noreturn]] static void panic(const char*)` method that dictates how the system terminates if a fatal, unrecoverable developer bug occurs.

**The Why:** This enforces the Single Responsibility Principle by decoupling "System Physics" (how the API compiles and crashes) from "Auditing" (how the API logs history). By checking these flags via `if constexpr`, evaluators and debug builds receive total safety. Production deployments can opt into a relaxed policy that physically deletes the branch checks, resulting in raw, zero-overhead execution speed without requiring massive `#ifdef` macros.

### [DESIGN-CONTRACTS-COMPILE_TIME]

**The Rule:** The SDK must enforce internal API preconditions using custom, strong-type wrappers (e.g., `docwire::not_null<T>`, `docwire::ranged<T>`) to shift logic checks to compile-time wherever mathematically possible. Standard library GSL is banned, as its enforcement relies on global macros that violate our dependency injection model.

Crucially, strong-type wrappers must expose two distinct ingestion paths to perfectly separate trusted developer logic from untrusted payload data:

1. **Trusted Assignment (Constructors / `operator=`):** Assumes the input is safe. These checks must be guarded by `if constexpr` using the injected `SafetyPolicy`. In `strict` mode, a violation triggers a fatal developer panic (e.g., `SafetyPolicy::panic()`). In `relaxed` mode, the compiler physically deletes the branch, resulting in raw, zero-overhead execution.

2. **Untrusted Ingestion (Fallible Factories):** Used for parsing external payloads. Wrappers must provide a static factory method (e.g., `static expected<ranged> validate(T input)`) that validates raw data using standard `if` statements. **These checks act as a strict security boundary and must never be optimized out, even if the policy is `relaxed`.** They gracefully return an `expected` error on failure.

**The Logging Boundary:** Fallible factories may optionally accept a stateful `Audit` object via a template parameter (e.g., `template <typename Audit> static expected<ranged> validate(Audit& audit, T input)`). The caller strictly dictates the logging behavior: it may pass its active `Audit` object to allow the wrapper to automatically log semantic constraints (e.g., min/max bounds), or it may pass a stateless `NullAudit` (or omit the argument) to ensure silent, zero-overhead failure during Speculative Parsing.

**The Why:** Strong types make APIs self-documenting and mathematically un-compilable if used incorrectly. A compile-time error is infinitely cheaper than a runtime check, even a morphing template one. By utilizing template-based `Audit` injection, the fundamental wrappers avoid physical header coupling with the telemetry infrastructure. Leaving the logging decision to the caller ensures the parser maintains absolute control over the Signal-to-Noise ratio of the Audit trail without duplicating bounds-checking logic.

### [DESIGN-PAYLOAD-ISOLATION]

**The Rule:** The `SafetyPolicy` template (e.g., `strict` vs `relaxed`) strictly governs internal developer API contracts. It must **never** be used to conditionally disable bounds-checking on untrusted external payloads (e.g., parsing raw bytes from a PDF or ZIP). Payload boundaries must always be rigorously checked and structurally return an `expected` error on failure.

**The Why:** A `relaxed` policy optimizes trusted developer logic. Applying it to untrusted I/O creates buffer overflows and critical security vulnerabilities (CVEs). Untrusted data is always assumed hostile and must never bypass structural validation.

### [DESIGN-METADATA-ANNOTATION]

**The Rule:** When attaching compile-time metadata to a payload, developers must use the metadata engine located in the `docwire::annotated::` namespace. Metadata tags themselves should reside in their respective domain namespaces (e.g., `fs::file_name`, `http::mime_type`), not a generic junk-drawer namespace.

* **The Wrapper:** Use `docwire::annotated::object<Payload, Tags...>`. It must provide an implicit conversion operator to its underlying Payload type, allowing it to cross API boundaries into generic algorithms seamlessly.

* **The Appender:** Use the `docwire::annotated::append(payload, tags...)` factory to add new metadata, which handles the underlying `std::tuple_cat` logic without manual template nesting.

* **The Concept:** Functions requiring specific metadata must enforce it via the `docwire::annotated::has<T, DomainTag>` Concept.

* **The Extractor:** Metadata must be extracted using the free function `docwire::annotated::get<DomainTag>(obj)` to avoid template keyword disambiguation.

**The Why:** Isolating the metadata feature into the `annotated::` namespace prevents global namespace pollution. The implicit conversion operator allows annotated types to behave transparently as their underlying payloads when metadata is not required. Using Concepts for extraction guarantees mathematically robust, order-independent structural subtyping with zero runtime overhead.

### [DESIGN-RESOURCE-LIFECYCLES]

**The Rule:** The instantiation and teardown of fallible OS resources (e.g., files, network sockets) must be strictly managed using lazy **Resource Factories**, ensuring safe execution and strictly ordered teardown. This must be enforced at compile-time via strict encapsulation.

1. **Universal Factory Template:** SDK developers must not write custom boilerplate factories. Instead, they must use the standard `docwire::resource_factory<Resource>` template, which securely stores constructor arguments in a tuple until execution.

2. **Strict Encapsulation (The Friendship Rule):**

   * **Resources:** Resource classes (e.g., `pdf::reader`) must have **private constructors** to physically prevent eager instantiation. They must declare `friend class docwire::resource_factory<Resource>;` to allow only the factory to create them.

   * **Factories:** The factory's `.create()` and `.destroy()` methods must also be **private**. The factory must declare the pipeline execution engine as a `friend` to ensure only the pipeline can trigger resource allocation.

3. **Mathematical Composition (Simultaneous Resources):** If a process requires multiple resources open *at the same time*, factories must be combined using the `+` operator (e.g., `f1 + f2`). This creates a composite Source that yields a tuple of resources to the pipeline.

4. **Execution Delegation:** Factories act exclusively as the "Source" of a pipeline. When the pipeline executes, the engine securely invokes the private `.create()` method, routes any `expected` errors, and mathematically guarantees that `.destroy()` is called in LIFO (Last-In, First-Out) order.

**The Why:** This guarantees exception-free, 100% leak-proof resource management. Standard C++ objects (like `std::ifstream`) handle failure either by throwing exceptions or quietly entering a silently failed 'zombie state'. By locking down constructors via `friend` access and forcing the use of factories, we mathematically prevent standard library exceptions and zombie objects from bypassing our strict expected error-routing physics. The API remains completely flat, composable, and integrates flawlessly into modern C++ lazy pipeline architectures.

```cpp
// ✅ GOOD: Strict Encapsulation and Factory Usage

class pdf_reader {
    // 1. Private constructor prevents eager instantiation
    pdf_reader(std::string_view path) { ... }
    
    // 2. Only the universal factory template can create this
    friend class docwire::resource_factory<pdf_reader>; 
public:
    expected<page, Error> read_page();
};

// 3. User creates a factory using the template alias
using reader_factory = docwire::resource_factory<pdf_reader>;

// 4. Composable factories for simultaneous resource consumption
auto doc_factory = reader_factory{"input.pdf"};
auto image_factory = image::reader_factory{"watermark.png"};

// Combined into a single Source yielding std::tuple<pdf_reader, image_reader>
auto composite_source = doc_factory + image_factory;
```

### [DESIGN-PIPELINE-ARCHITECTURE]

**The Rule:** Data processing and transformations must be chained using a strictly lazy, exception-free pipeline architecture via `operator|`. This architecture relies on five mandatory constraints:

1. **Sources, Steps, and Destinations (Nouns):** A pipeline is an assembly line of objects.

   * **Source:** The input of the pipeline. This can be a Resource Factory (when lifecycle management is needed), or standard data containers (e.g., `std::span`, `std::vector`).

   * **Step:** Intermediate transformations must be stateful class objects inheriting from `docwire::pipeline_step`, instantiated universally with braces (e.g., `encryptor{key}` or `count_pages{}`).

   * **Destination:** The pipeline must end with an explicit terminal node holding a reference to the output variable (e.g., `docwire::destination{my_var}`).

2. **Compile-Time Type Safety:** The `operator|` strictly enforces type compatibility. The output type of the Left-Hand Side must directly match the accepted input parameter of the Right-Hand Side's `operator()`. Incompatible connections will yield compile-time errors.

3. **Lazy Composition & Ownership:** The `operator|` must never execute logic or I/O. It acts exclusively as a graph builder. It must capture step configurations by value (moving temporaries) to mathematically prevent dangling references.

4. **Zero-Copy Payloads:** While the configuration blueprint is moved by value, the actual data payload must be passed using Perfect Forwarding (`Input&&` via `std::forward`). Data buffers and OS resources must never be copied between pipeline steps.

5. **Explicit Execution & Friendship:** Pipelines must never execute eagerly. Execution must be triggered explicitly via the function call operator `pipeline()`. The pipeline engine uses its friend access to safely instantiate the source factories, routes the data by reference, captures the result in the destination, safely invokes factory destruction, and returns an `expected` error.

**The Why:** This architecture perfectly mirrors modern C++ Sender/Receiver physics. Universally using `Noun{}` creates a highly readable, consistent syntax. Deferring execution allows the graph to be built safely on one thread and executed on another. Capturing the blueprint by value mathematically eliminates Use-After-Free (dangling reference) bugs when pipelines are returned from functions. Perfect forwarding the payload guarantees non-copyable OS resources and massive buffers are routed with zero overhead. Finally, using a terminal Destination node with `operator|` avoids the fatal operator precedence traps and immediate-execution side effects inherent to `operator>>`. The architecture provides 100% memory safety, strict type-checking, and zero-cost abstraction.

```cpp
// ✅ GOOD: A lazy, strictly typed, zero-copy pipeline execution graph.

int page_count;
std::vector<std::byte> raw_buffer = get_data();

// Example A: Factory Source (Engine uses `friend` access to safely run I/O)
auto factory_pipe = pdf::reader_factory{"doc.pdf"} 
                  | pdf::decryptor{my_key}
                  | pdf::count_pages{} 
                  | docwire::destination{page_count};
auto err1 = factory_pipe();

// Example B: Raw Data Source (Type-checked at compile time)
auto memory_pipe = raw_buffer 
                 | security::validate_checksum{} 
                 | docwire::destination{page_count};
auto err2 = memory_pipe();
```

## API (SDK Boundaries & Interfaces)

### [API-BOUNDARY-INTENTIONAL]

**The Rule:** The Public API strictly exposes Domain Interfaces and Core Utilities. It must hide all transient execution states (parser state machines, raw buffers).

**The Why:** Exposing the "What" empowers the user. Exposing the "How" creates a brittle, impossible maintenance contract that paralyzes future optimization.

### [API-VERSIONING-DATECENTRIC]

**The Rule:** The SDK strictly follows a Date-Centric "Release Early, Release Often" versioning model, explicitly rejecting Semantic Versioning (SemVer). The project maintains a single continuously evolving main branch, and releases are tagged by date.

**The Why:** SemVer implies a promise to maintain outdated major-version code branches, draining engineering resources. Date-based versioning provides transparent chronological progress and forces the engineering focus entirely on improving the head branch, maximizing innovation velocity.

### [API-STABILITY-EVOLUTION_FIRST]

**The Rule:** A "Good API" strictly outranks a "Stable API." We will aggressively refactor, rename, or break public C++ APIs if the change improves architectural purity, deterministic performance, or developer ergonomics. Legacy API support is not guaranteed in the main branch.

**The Why:** API stability is Priority #6 in our hierarchy. Tying our hands to a flawed API paralyzes the project. Customers who require strict API consistency or frozen feature sets must explicitly utilize Long-Term Support (LTS) agreements rather than dictating the velocity of the main open-source branch.

### [API-ABI-UNSUPPORTED]

**The Rule:** Application Binary Interface (ABI) stability is fundamentally non-existent and explicitly unsupported.

**The Why:** Guaranteeing C++ ABI stability requires hiding data behind opaque pointers and virtual dispatches, which completely destroys CPU inlining and violates our O(1) zero-cost abstraction goals. Because the SDK heavily utilizes C++20 templates and Concepts, users are expected to compile the SDK from source alongside their application.

### [API-DEPENDENCY-HIDDEN]

**The Rule:** Public headers must never `#include` 3rd-party library headers.

**The Why:** Including 3rd-party headers in public interfaces pollutes the user's global namespace and forces them to manage transitive dependencies.

### [API-DOCUMENTATION-INLINE]

**The Rule:** Every public entity (class, concept, function, template) exposed in the SDK boundary must be strictly documented using inline Doxygen/DocBlock comments directly in the header file. Documentation must include `@brief`, `@param`, `@return`, and explicit `@note` tags defining the exact memory lifecycle expectations (e.g., *"@note The returned span points directly into the source payload,"* or *"@note The returned container's lifetime is bound to the injected Allocator"*).

**The Why:** In a rapidly evolving, date-versioned SDK, external documentation sites immediately drift from the codebase. By enforcing Doxygen at the header level, the API documentation is version-controlled, code-reviewed, and shipped in the exact same Pull Request as the feature itself. Explicitly documenting lifecycle expectations prevents dangling pointers in a zero-heap environment.

### [API-NAMING-PIPELINE_NOUNS]

**The Rule:** Pipeline steps, adaptors, and destinations must be named using Object-Oriented nouns or agent nouns (e.g., `encryptor`, `extractor`, `count_pages_step`, `destination`), strictly avoiding raw action verbs (e.g., `encrypt`, `extract`).

**The Why:** Because the SDK utilizes C++17 Class Template Argument Deduction (CTAD) to instantiate pipeline steps directly via braces (e.g., `encryptor{key}`), they are structurally objects, not function calls. Naming them as nouns creates a highly readable, physical 'assembly line' syntax. Crucially, it removes the mental friction of seeing a verb-function like `parse()` and wondering if it executes immediately or lazily. Nouns explicitly communicate deferred execution and state ownership.

### [API-STATE-INJECTION]

**The Rule:** Global state, static mutables, and Singleton patterns are strictly banned across the SDK. All external side-effects and resource managers—including Memory Allocators, Diagnostic Sinks (for user warnings), and Loggers (for internal telemetry)—must be explicitly passed into the pipeline via Dependency Injection.

**The Why:** Singletons are globally mutable state. Utilizing them instantly destroys the thread-safety, lock-free concurrency, and reentrancy of the SDK. By explicitly passing a Context object down the call stack, the architecture guarantees that parallel execution pipelines remain physically isolated and highly testable, while preventing function signature bloat.

### [API-HOST-SOVEREIGNTY]

**The Rule:** The SDK must never police, constrain, or dictate the architectural choices of the user's injected dependencies (e.g., custom `Audit` implementations, custom `SafetyPolicy` panic hooks, or custom `Allocator` routing). While the SDK strictly bans exceptions, mutexes, and globals internally, users are entirely free to utilize them within the code they inject across the SDK boundary.

**The Why:** The Host Application is sovereign. If an enterprise user chooses to inject an `Audit` object backed by a global thread-locked logging singleton, or a `SafetyPolicy` that throws `std::runtime_error`, that is their deliberate architectural trade-off. The SDK's guarantee is simply that it will not introduce its *own* bottlenecks; it must fully support the user's right to introduce theirs.

## BUILD (Physical Compilation & Packaging)

### [BUILD-INLINE-LIMITS]

**The Rule:** Use the standard `inline` keyword for headers, but strictly ban forced inlining attributes (`__forceinline`) unless backed by explicit profiling data.

**The Why:** Over-inlining causes Instruction Cache (I-Cache) thrashing. We trust the compiler's heuristics to balance inlining limits.

### [BUILD-DEFAULT-HEADER]

**The Rule:** By default, all pure C++ logic (templates, data structs, inline algorithms) belongs in the header files.

**The Why:** Modern compilers achieve peak performance through aggressive cross-function inlining. Pushing pure C++ logic into `.cpp` files blocks this optimization.

### [BUILD-QUARANTINE-CPP]

**The Rule:** A `.cpp` file is strictly a quarantine zone. Code is only moved to a `.cpp` file if it: 1) Hides OS/3rd-party dependencies, 2) Anchors global/static state (ODR compliance), or 3) Compiles kernels with aggressive, private hardware flags.

**The Why:** This removes subjective debates about code placement. If code pollutes the user's environment, state, or compilation flags, it gets quarantined in a translation unit.

### [BUILD-FUTURE-MODULES]

**The Rule:** Public headers must act as clean interfaces, completely devoid of macro pollution and transitive C-library includes.

**The Why:** This quarantine architecture guarantees that our public headers can seamlessly wrap into an export module block for future C++20 Module migrations without refactoring.

### [BUILD-COMPILER-STRICTNESS]

**The Rule:** The CMake build system must enforce strict compiler warnings and treat all warnings as errors (`-Wall -Wextra -Werror` on GCC/Clang, `/W4 /WX` on MSVC). The build must completely disable compiler-specific language extensions.

**The Why:** An uninitialized variable or implicit narrowing conversion warning is a bug waiting to happen. By treating warnings as errors, we force the compiler to reject sloppy code before a human ever has to review it. Disabling extensions guarantees our C++20 code remains universally portable across compilers.

### [BUILD-LAYOUT-MAPPING]

**The Rule:** The physical repository structure must strictly mirror the C++ namespace and type hierarchy.

* **Mapping & Prefix Ban:** Namespaces dictate directories; Classes/Structs/Functors dictate files (e.g., the functor `detect` inside the namespace `content_type::by_signature` must physically reside at `src/content_type/by_signature/detect.hpp`). Prefixing type names to fake namespaces (e.g., naming a struct `pdf_parse` instead of putting `struct parse` in `namespace pdf`) is strictly banned.

* **Extensions:** All C++ header files must exclusively use the `.hpp` extension to differentiate them from C-compatible headers.

**The Why:** A strict mapping means developers never have to run search queries to find a file; the C++ type name is literally the file path. Banning prefixes forces proper C++ namespace usage and keeps identifiers clean.

### [BUILD-LAYOUT-UMBRELLAS]

**The Rule:** Internal implementations must be highly granular (one class, template, or functor per header file). However, the public API must hide this granular tree by providing Boost-style aggregate "umbrella" headers at the root of each domain (e.g., `src/content_type.hpp` simply includes all necessary internal content type detection headers).

**The Why:** Granular internal headers guarantee fast, highly parallelized compile times and clean git commits. Umbrella headers provide excellent developer ergonomics, allowing end-users to consume entire modules with a single, clean `#include` statement without needing to understand our internal file tree.

### [BUILD-LINK-AGNOSTIC]

**The Rule:** The SDK must remain agnostic and build flawlessly as both a static library and a shared/dynamic library.

**The Why:** Package managers like `vcpkg` dictate the build triplet. While static linking with LTO is optimal for our inlining goals, we cannot force the host application's architecture and must support standard shared boundaries.

## MEMORY (Allocation & Teardown)

*Context: DocWire operates a "Zero-Heap Pipeline." During active parsing, standard OS heap allocations (`new`, `malloc`, `std::vector`) are mathematically too slow and cause fragmentation. We achieve deterministic, O(1) performance by routing all memory through a custom bump-allocator (the arena) and strictly managing the OS stack.*

### [MEM-HOST-OWNERSHIP]

**The Rule:** The Host Application (the top-level execution scope calling the SDK) is the exclusive owner of the OS boundary. It is strictly and solely responsible for allocating massive Input Payloads (via `mmap` or heap `std::vector`) and instantiating the Memory Allocators/arenas. These resources must be passed into the pipeline strictly via Dependency Injection (the execution context).

**The Why:** Domain structs (e.g., the `pdf::document` returned by a parser) are lightweight, transient, and immutable; they must never manage OS resources or their own memory pools. Centralizing OS-level allocations at the host boundary guarantees that the DocWire pipeline remains a pure, zero-allocation, zero-copy state machine and honors the `[API-STATE-INJECTION]` rule.

### [MEM-DATA-COMPOSITION]

**The Rule:** Developers must not write manual memory management logic (`new`/`delete`). The internal composition of data structures must strictly adhere to these three classifications:

* **Fixed Data (Value Semantics):** Types with a known, compile-time size (e.g., `int`, enums, fixed structs) must be composed by value.

* **Variable Data (Allocator-Aware Containers):** Dynamically sized internal data (e.g., child arrays built during execution) must use standard C++ containers parameterized with the explicitly injected allocator (e.g., `std::vector<T, Allocator>`). Default-allocated containers that fall back to the OS heap are strictly banned.

* **Input Payload (Zero-Copy Views):** References to the raw input stream or external assets must exclusively use non-owning view types (e.g., `std::span<const std::byte>`, `std::string_view`, or `std::span<const T>`).

**The Why:** Aligning data composition directly with modern C++ value semantics and allocator-aware standard containers eliminates manual memory leaks. It perfectly satisfies the `[MEM-EXPLICIT-PROPAGATION]` rule while giving developers the freedom to use standard C++ idioms without rigid aliases or overly restrictive type boundaries.

### [MEM-TEARDOWN-OPTIMIZER_DRIVEN]

**The Rule:** Data structures and intermediate states allocated via the injected `docwire::arena::allocator` must be designed to be Trivially Destructible whenever mathematically possible. Domain structs must strictly avoid acquiring external system resources (e.g., file handles, OS mutexes) that require complex destructor logic.

**The Why:** The `arena` achieves $O(1)$ memory release simply by resetting an internal bump pointer. However, C++ still executes destructors for objects (like `std::vector`) when they go out of scope. If the internal domain structs are cleanly designed and manage no external OS resources, the compiler's Dead Code Elimination (DCE) will completely optimize away the destruction loops, making the entire pipeline's teardown mathematically instantaneous.

### [MEM-STACK-LIMITS]

**The Rule:** The physical `sizeof()` any single local variable, struct, or fixed-size array allocated on the C++ stack must strictly not exceed **1 Kilobyte (1024 bytes)**. Any local working state requiring more than 1KB must be requested from the arena.

**The Why:** Parsers rely heavily on deep call stacks and recursion. Because standard OS thread stacks are limited (e.g., 1MB on Windows), allocating large fixed buffers on the stack will cause unpredictable Stack Overflow crashes on deeply nested documents.

### [MEM-ARENA-LARGE_BYPASS]

**The Rule:** The `arena::allocator` class acts as the universal memory provider, abstracting all size-based routing from the developer. It must implement a Large Object Bypass: requests smaller than a defined threshold (e.g., 512KB) use the $O(1)$ bump-pointer. Requests exceeding this threshold must dynamically allocate a dedicated OS Heap block, tracked internally by the Arena for bulk deallocation upon teardown.

**The Why:** Developers using allocator-aware containers (like `std::vector<T, Allocator>`) are freed from guessing if an array will grow "too big" for the arena. The arena automatically protects its contiguous internal chunks from fragmentation by routing massive allocations directly to the OS, while still maintaining a single, safe memory lifecycle.

### [MEM-ALLOCATOR-FLEXIBILITY]

**The Rule:** SDK components that require dynamic memory must be templated on an `Allocator` type (typically via the injected execution context) to allow end-user substitution. The SDK must provide a high-performance default (e.g., `docwire::arena::allocator`). Tuning parameters for this default arena (chunk size, bypass limits) must be configured via runtime constructor arguments, never template arguments.

**The Why:** We must assume enterprise users (cloud-scale backend servers, embedded edge devices) have highly specific, proprietary memory managers. Providing a default `arena::allocator` offers a zero-configuration "Golden Path" for 99% of users, while strict template injection preserves standard C++ flexibility for the 1%. Keeping tuning parameters in the constructor prevents catastrophic type-system fragmentation.

### [MEM-EXPLICIT-PROPAGATION]

**The Rule:** Every function, method, or constructor that triggers dynamic memory allocation must explicitly require an Allocator instance. Hidden global memory pools or default heap fallbacks are strictly banned.

* **Object State:** If a class requires dynamic memory for its own internal state, its constructor must require the allocator.

* **Worker Methods:** If a class or free function only allocates memory to generate an output the executing method must require the allocator as a parameter.

* **Propagation Mechanics:** Because our `docwire::arena::allocator` is stateful and lacks a default constructor, the allocator must be passed by const reference (e.g., `const Allocator& alloc`) and manually propagated down to all dynamic children.

**The Why:** By demanding the allocator exactly where the allocation happens, we guarantee pure Dependency Injection, making hidden heap allocations impossible and memory lifetimes instantly visible. Furthermore, passing by `const Allocator&` matches STL conventions and prevents expensive copies of stateful allocators, ensuring the exact same physical memory pointer cascades safely down the entire parsed tree.

### [MEM-LAZY-EVALUATION]

**The Rule:** Processing algorithms must never eagerly decompress, decode, or transform payload data (e.g., ZIP streams, Base64 strings, encrypted blocks) during the initial structural parsing phase. Data transforms must evaluate lazily, executing only when explicitly requested by the caller.

**The Why:** A user might only want to read the metadata of a 50MB PDF. If an algorithm eagerly decompresses all image streams, it wastes massive amounts of CPU and memory. Lazy evaluation guarantees we only pay the cost for data the user actually touches.

### [MEM-COPY-ZERO_DEFAULT]

**The Rule:** If an algorithm reads contiguous data that does not require physical transformation, it must return a non-owning view (`std::span`, `std::string_view`) pointing directly to the source payload.

**The Why:** Prevents useless heap allocations and memory copying for plaintext tokens, dictionary keys, and uncompressed byte blocks, keeping the algorithm mathematically $O(1)$ regarding data size.

### [MEM-IDIOMATIC-TRANSFORMS]

**The Rule:** When an algorithm *must* physically transform data (e.g., decompressing a ZIP chunk), it must not use C-style pre-allocated output buffers. Instead, the algorithm must utilize the injected `Allocator` and return an idiomatic C++ container by value (e.g., `std::vector<T, Allocator>`).

**The Why:** C-style output buffers force the caller to guess sizes or make two passes. Returning a vector parameterized with the injected allocator is lightning-fast, 100% memory-safe, and provides a beautiful, modern C++ API.

### [MEM-COROUTINE-RESTRICTION]

**The Rule:** C++20 Coroutines (`co_await`, `co_yield`) are strictly banned in core data pipelines and parsers unless their `promise_type` is explicitly overridden to allocate the coroutine state frame from the `docwire::arena::allocator`.

**The Why:** By default, the C++ standard mandates that coroutine suspension frames are dynamically allocated on the OS heap. While compilers attempt Heap Allocation Elision (HALO) to optimize this onto the stack, it frequently fails across translation boundaries or in complex loops. A failed HALO will silently trigger an OS heap allocation, violating our deterministic memory architecture and triggering the `[TEST-HEAP-TRIPWIRE]`. Explicit State Machine structs are preferred for parsing state.

## ERR (Error Handling & Validation)

### [ERR-ROUTING-VALUE_BASED]

**The Rule:** Standard C++ exceptions (`throw`, `try`, `catch`) are strictly banned. All fallible algorithms must return a value-based result utilizing the internal template `docwire::expected<T, E = docwire::error_category>`. The error payload must be a Trivially Copyable type (e.g., an enum or bitmask) representing coarse-grained control-flow states (e.g., `program_logic`, `uninterpretable_data`, `network_failure`).

**The Why:** Maintaining hundreds of specific error codes creates maintenance hell. Coarse-grained categories tell the caller how to route the flow (Abort, Retry, Skip) while keeping the return value register-fast. Using a custom expected template with a defaulted error argument provides a terse API (`expected<page>`) while allowing us to strictly dictate access violation physics.

### [ERR-CONTEXT-DELEGATION]

**The Rule:** Rich error context (e.g., dynamic strings, nested call stacks, memory-backed variable captures) must never be stored inside or linked from the `expected` return object. Whenever a failure occurs, the algorithm must push its rich diagnostic data to the injected `Audit` interface *before* returning the trivially copyable error code.

**The Why:** Keeping the return type strictly for control flow preserves the zero-heap, $O(1)$ CPU return physics. Delegating the rich context to the independent `Audit` channel ensures that critical telemetry (warnings, skipped corrupted files) is preserved in the application's audit trail even if the pipeline cleanly recovers from the error.

### [ERR-CONTRACT-VIOLATIONS]

**The Rule:** When a developer violates an internal API contract, the SDK's response must follow this hierarchy based on the injected `SafetyPolicy`:

1. **Compile-Time Priority:** Enforced at compile-time using strong types per `[DESIGN-CONTRACTS-COMPILE_TIME]`.

2. **High-Level Domain APIs (Return `expected` is possible):**

   * **`strict` mode:** The SDK evaluates the `SafetyPolicy::check_*` flag, pushes a diagnostic trace to the stateful Audit interface, and safely returns `unexpected(program_logic)`.

   * **`relaxed` mode:** The check is erased from the compiler's view via `if constexpr`. Pure Undefined Behavior (UB).

3. **Low-Level Standard Mimics (Return `expected` is impossible):**
   For mechanical primitives (e.g., `expected::value()`, `span::operator[]`), changing the return signature is forbidden, and DocWire explicitly bans throwing internal exceptions.

   * **`strict` mode:** The SDK must trigger a fatal panic by delegating directly to the stateless template policy (e.g., `SafetyPolicy::panic()`). The SDK defaults this policy to `std::abort()`, but the host application is free to inject a custom policy.

   * **`relaxed` mode:** The check is erased. Pure Undefined Behavior (UB).

**The Why:** This hierarchy flawlessly balances safety, standard C++ expectations, and peak performance. It guarantees recoverable logic bugs are safely logged and routed via `expected`, while unrecoverable violations in low-level primitives trigger configurable, zero-overhead fatal panics. By delegating the failure context to the `Audit` stream before returning the error, the SDK proves the mathematical impossibility of the user's request (exonerating the SDK from "crash blame"). This also grants the host application total control: their injected `Audit` implementation can optionally trigger a debug trap, throw an exception, or safely terminate the process on their own terms.

### [ERR-RECURSION-LIMIT]

**The Rule:** Any parser utilizing recursive descent or hierarchical tree traversal must implement and strictly enforce a hardcoded Maximum Depth Limit (e.g., `consteval int max_depth = 256;`). The current depth must be passed down the call chain, and exceeding the limit must immediately abort the parse.

**The Why:** Maliciously crafted input files can weaponize deeply nested structures to intentionally trigger OS Stack Overflow crashes. Enforcing a strict limit mathematically bounds worst-case execution time and guarantees system stability.

## CONCURRENCY (Concurrency and Thread-Safety)

### [CONCURRENCY-PRIMITIVE-QUARANTINE]

**The Rule:** Domain objects, processing pipelines, and custom allocators (like the `docwire::arena::allocator`) are strictly prohibited from containing OS synchronization primitives (`std::mutex`, `std::shared_mutex`, `std::condition_variable`). Synchronization state must live exclusively in the infrastructure/execution layer (e.g., Thread Pools, Task Schedulers).

**The Why:** We rely on the C++ compiler's Dead Code Elimination to achieve O(1) teardown (`[MEM-TEARDOWN-OPTIMIZER_DRIVEN]`). Embedding OS synchronization primitives (which require complex destructors) inside arena-allocated data structures forces the compiler to abandon O(1) elision and execute a slow O(N) teardown. Furthermore, embedding locks inside allocators or data structures destroys deterministic O(1) cache performance during active parsing. Infrastructure manages threads; data pipelines just execute logic.

### [CONCURRENCY-SHARING-MODEL]

**The Rule:** The SDK does not support "Shared Mutable State." Cross-thread data pipelines must exclusively use one of two lock-free paradigms:

**Strict Ownership Transfer:** A mutable data structure (and its backing Arena) is owned by exactly one thread at a time.

**Immutable Sharing:** Once a thread finishes building a data structure, it transitions to a strictly "Read-Only" state. Subsequent concurrent operations across multiple threads must execute lock-free by reading this immutable data.

**The Why:** Shared mutable state forces the use of locks, which inherently bottleneck parallel throughput and cause deadlocks. By enforcing spatial boundaries (Thread-Local Arenas) or temporal boundaries (Phase 1: Write, Phase 2: Read-Only), we achieve maximum multicore scaling without the overhead of synchronization primitives.

### [CONCURRENCY-ALGORITHM-REENTRANCY]

**The Rule:** All parsing, processing, and transformation algorithms must be strictly reentrant and thread-safe. They must not rely on hidden global state, static mutable variables, or singletons. All state required for an algorithm to execute must be passed in via arguments (e.g., passing the context and allocator).

**The Why:** If a worker algorithm relies on global state, it implicitly forces cross-thread coupling, making it impossible to run multiple instances of the algorithm in parallel without introducing locks. Pure, state-injected functions guarantee infinite, lock-free parallel scaling.

### [CONCURRENCY-SHARED-NOTHING]

**The Rule:** SDK processing components must be strictly single-threaded and isolated. Multi-threading is achieved by instantiating independent component pipelines per thread. Massive shared assets (Fonts, LLM weights, static dictionaries) must be fully initialized by the main thread, frozen as `const`, and shared across workers strictly as immutable, read-only references. Mutexes and atomics are banned in the parsing pipeline.

**The Why:** Mutexes and atomics destroy CPU cache coherency and pipeline throughput. By enforcing a "Shared Nothing" execution model and "Immutable Sharing" for massive data, the SDK scales linearly across CPU cores without synchronization overhead.

## DEPENDENCIES (3rd-Party Integration)

### [DEPENDENCY-SELECTION-CRITERIA]

**The Rule:** When evaluating 3rd-party libraries, preference is strictly given to components that natively align with our architecture (allocator-aware, C-style interfaces, no exceptions). Header-only libraries are preferred only if they meet this criteria.

**The Why:** A highly compliant, allocator-aware C library (like zlib) is infinitely more valuable to a zero-heap pipeline than a modern, header-only C++ library that throws exceptions and calls `new` under the hood.

### [DEPENDENCY-CONTAINMENT-REALITY]

**The Rule:** Hostile dependencies (those that throw exceptions or allocate globally) must be quarantined inside private `.cpp` translation units. The wrapper must translate exceptions into `docwire::expected` and manage internal states. We explicitly acknowledge that while this protects our API boundary, it does not mitigate the runtime performance penalty of stack unwinding or internal heap usage.

**The Why:** Containment stops architectural pollution from spreading to the user's codebase, but it cannot bend the laws of physics. We accept the localized performance degradation of the wrapped library as explicitly tracked technical debt.

### [DEPENDENCY-LIFECYCLE-STRATEGY]

**The Rule:** Integration of 3rd-party code must follow a strict three-phase progression to balance business needs with technical physics:

* **Isolate & Ship:** Quarantine the library, accept the performance hit, and deliver the feature.

* **Profile & Patch:** If profiling proves the library is a bottleneck, fork it and apply minimal compiler flags or allocator overrides (e.g., hooking into its internal memory callbacks).

* **Reimplement:** Only rewrite the logic from scratch if profiling proves the library's internal architecture fundamentally blocks DocWire's peak throughput goals and users are actively utilizing the feature.

**The Why:** Reimplementing complex formats (like image decoding or compression) from scratch delays deliverables by months. This lifecycle guarantees we only spend engineering hours rewriting the exact components that actually limit system scale.

### [DEPENDENCY-MEMORY-HOOKING]

**The Rule:** If a 3rd-party C library performs internal memory allocation and supports custom allocator callbacks (e.g., `zlib`'s `zalloc`/`zfree`), developers must wire those callbacks directly into the injected allocator. Standard OS `malloc`/`free` calls are strictly banned even inside 3rd-party boundaries whenever hooking is possible.

**The Why:** Unhooked 3rd-party libraries act as memory leaks in our zero-heap architecture, causing hidden fragmentation and violating the `[TEST-HEAP-TRIPWIRE]`. Hooking their allocators brings them under the umbrella of our $O(1)$ teardown physics.

## TESTING (Validation & Enforcement)

### [TEST-HEAP-TRIPWIRE]

**The Rule:** The core unit testing framework must globally override `operator new` and `operator delete`. During the execution of the core parsing test suites, a global flag must actively ban heap allocations. If any code path attempts to dynamically allocate OS memory, the overridden operator must immediately trigger `std::abort()` with a stack trace.

**The Why:** Human code reviewers will inevitably miss a hidden `std::string` instantiation. This tripwire provides undeniable, mathematical proof that our parsing pipeline remains a "Zero-Heap" environment.

### [TEST-SANITIZER-GATE]

**The Rule:** The Continuous Integration (CI) pipeline must enforce dynamic analysis gates across three strict categories of defect detection before any code is merged:

1. **Memory Correctness:** (e.g., ASan, Valgrind Memcheck) to catch buffer overflows, use-after-free, and leaks.

2. **Undefined Behavior:** (e.g., UBSan) to catch integer overflows, unaligned pointers, and strict aliasing violations.

3. **Concurrency:** (e.g., TSan, Helgrind) to catch data races and deadlocks.

**The Why:** A zero-copy parser operating on raw byte spans and using custom Arena memory is mathematically fast, but structurally dangerous. The C++ compiler cannot statically verify lifetime bounds across injected Arenas. We must mandate dynamic instrumentation to mathematically prove the pipeline is safe from memory and concurrency defects.

### [TEST-STATIC-ANALYSIS]

**The Rule:** The Continuous Integration (CI) pipeline must enforce automated static analysis as a strict gating mechanism on all code contributions. The analysis must be configured to statically detect lifetime/borrowing violations, enforce modern C++ idioms, and flag dead code or excessive cyclomatic complexity. The configuration profiles dictating these checks must be explicitly version-controlled alongside the codebase. Tooling choices (e.g., `clang-tidy`, `cppcheck`) may evolve, but the automated enforcement gate must remain absolute.

**The Why:** Catching semantic bugs, style violations, and potential undefined behavior at compile-time prevents easily avoidable defects from ever reaching the dynamic testing phases (like sanitizers or fuzzing). Treating static analysis as a strict, automated pipeline gate ensures a uniform baseline of structural code quality without relying on human reviewers to manually spot hidden C++ footguns.

### [TEST-STYLE-AUTOMATED]

**The Rule:** The codebase strictly defers to the C++ Core Guidelines for naming and layout conventions. Human code reviews must never involve debates about code style, indentation, or bracket placement. Style compliance must be enforced entirely by automated CI mechanisms.

**The Why:** Delegating 100% of stylistic checks to automated tooling reclaims thousands of hours of engineering time. Pull Request reviews must focus entirely on architecture, memory safety, and logical correctness, never on syntax formatting.

### [TEST-PERFORMANCE-DETERMINISTIC]

**The Rule:** The CI pipeline must enforce performance regression testing using deterministic instruction-counting tooling (e.g., Callgrind) rather than wall-clock time. Performance limits must be hardcoded as constants in the test suite (e.g., `REQUIRE(instructions_executed < 15000)`). The CI pipeline must strictly fail if a code change exceeds these hardcoded instruction limits.

**The Why:** Wall-clock time fluctuates wildly in shared cloud CI environments, causing flaky tests. Instruction counting is mathematically deterministic. Comparing against hardcoded constants ensures performance tests act exactly like functional unit tests, preventing O(N2) regressions without false positives.

### [TEST-HOSTILE-FUZZING]

**The Rule:** The CI pipeline must integrate continuous fuzzing (e.g., `libFuzzer`, `AFL++`) targeting all public parser entry points that consume untrusted payloads. Fuzz targets must run with Memory and Undefined Behavior sanitizers enabled.

**The Why:** Unit tests only test the bugs a developer can imagine. Parsers are actively targeted by malicious actors using malformed edge-cases. Continuous fuzzing provides mathematical proof that the `[DESIGN-PAYLOAD-ISOLATION]` boundaries hold and that the SDK will safely return `expected` instead of crashing on poisoned data.
