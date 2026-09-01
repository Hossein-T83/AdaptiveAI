---
title: "💡 ILO 1: Technical Quality"
preview_title: "💡 ILO 1"
layout: ilo
order: 1
---

<!-- markdownlint-disable-file MD033 -->

As a programmer, I develop high-quality technical work using a structured approach, so that it is fit for purpose: aligned with the project's goals, meeting its requirements, and well-structured, reliable, maintainable and performant.

## 🔍 Expected Evidence

Here are some guidelines on how you could evidence your work on the different levels of this ILO. The **full text of the rubric is leading the assessment** of your work. These are just examples, clarifications and tips. When the rubric is not clear, please discuss it with staff.

Learning log evidence of technical work, with requirements/acceptance criteria (e.g. linked Jira tickets), code snippets or diagrams illustrating key structure, and where relevant testing or profiling results.

## 💡 ILO 1: Technical Quality

<a name="poor"></a>
🔴 **[ILO 1 Poor]**

> Carries out technical work that is aligned with the project's goals.

<a name="insufficient"></a>
🟠 **[ILO 1 Insufficient]**

> Defines requirements for the work, with clear, objective and verifiable criteria for when they are met.

<a name="sufficient"></a>
🟡 **[ILO 1 Sufficient]**

> Delivers work that meets those requirements. Takes a structured approach: examines the constraints and opportunities of the existing codebase, engine and tools. Where relevant, illustrates the structure and key implementation of the work (for example with a code snippet or diagram).

<a name="good"></a>
🟢 **[ILO 1 Good]**

> Justifies the key technical decisions, weighing them against alternative approaches where these were considered supported through research and investigation. Evaluates the quality of the work critically, explaining whether it is well-structured, reliable and maintainable, and points out its strengths and limitations.

<a name="excellent"></a>
🔵 **[ILO 1 Excellent]**

> Shows understanding beyond what the task required. Acts on the identified limitations to improve the work: iterates on it and hardens it, for example by addressing edge and failure cases or improving performance with the help of profiling results. Shows that the work measurably improved as a result, and recognises where efficiency also serves sustainability, such as lower energy use.

Consider the following criteria:

* **Consistent Coding Style**: Follow a consistent coding style guide such as Google C++ Style Guide or LLVM Coding Standards to ensure readability and maintainability.
* **Comments and Documentation**: Document your code thoroughly using comments, especially for complex algorithms, data structures, and non-obvious logic. Also, consider generating documentation using tools like Doxygen.
* **Unit Testing**: Write comprehensive unit tests to validate the functionality of individual components. Tools like Google Test and Catch2 can help with unit testing.
* **Modularization**: Break your code into modular components with well-defined interfaces. This promotes reusability and simplifies maintenance.
* **Error Handling**: Implement robust error handling mechanisms to gracefully handle unexpected situations and provide meaningful error messages.
* **Memory Management**: Use smart pointers (such as std::unique_ptr and std::shared_ptr) and RAII (Resource Acquisition Is Initialization) to manage memory safely and automatically.
* **Static Code Analysis**: Employ static code analysis tools like Cppcheck, Clang Static Analyzer, or PVS-Studio to detect potential bugs, code smells, and adherence to coding standards.
* **Version Control**: Utilize version control systems like Git to track changes, collaborate with others, and revert to previous versions if needed.
* **Performance Optimization**: To meet the target performance, profile your code to identify performance bottlenecks and optimize critical sections while maintaining readability and maintainability.
