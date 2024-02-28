# IPK 2023/2024
Welcome to the IPK projects guideline repository.

## Goal
According to the assignment, you need to create an application communicating via a network.
You can implement what is not explicitly stated in the assignment according to your choice; however, describe the significant design decisions in the documentation.

## Coding Instruction
You can develop Projects 1 and 2 in C/C++ and C#.
If you decide to implement the project in C++, let it be by the C++20 standard.
If you decide to implement the project in C#, make your implementation .NET 8+ compatible and out-of-the-box runnable on the reference virtual machine with the provided developer environment active.
The use of object-oriented programming is required when implementing the project in languagues such as C++ or C#.
Common Lisp implementations will be tested using SBCL.

`Makefile` is mandatory for all project variants and programming languages.
Compilation of your projects will be done by calling `make`.
For C/C++, compiling with `Makefile` is native, for C#, you can call the standard `dotnet build` in the appropriate phony.
For Common Lisp, you can call `(asdf:make)`.
Using `Makefile` is to make the compilation of projects agnostic to the programming language used.

Among the allowed libraries are:

### C/C++
* BSD sockets API
* `libpcap`
* `libnet`
* or any standard header files present on the reference machine (e.g., `arpa/inet.h`)
* any testing framework of your choice
* any utility library not directly solving the core of the project, assuming you include it directly with your source code

### C#
* anything included in the base SDK
* any NuGet package not directly solving the core of the project (e.g., `xunit`, `Microsoft.Extensions.*`, `CommandLineParser`)
* `SharpPCAP`

The implementation should mark platform-dependent constructs (e.g., will only work on Linux).

Project source codes should be usable and meaningful, adequately annotated and formatted, and separated into appropriate methods and modules.
The program should include help to inform the user about the program's operation and parameters.
Any program errors should be intuitively described to the user.
The application shall not terminate with a `SEGMENTATION FAULT` or other violent system termination (e.g., division by zero).

An essential part of this project is demonstrating your software engineering skills.

> <span style="color:orange">Please use **private** [Git](https://git-scm.com/) repository on the faculty [Gitea portal](https://git.fit.vutbr.cz/) when versioning your project.
Make sure to give access to all [IPK instructors](https://www.fit.vut.cz/study/course/231006/.en).
</span>

Your repository should have *multiple* commits (preferably using [semantic commit messages](https://www.conventionalcommits.org/en/v1.0.0/)) illustrating your development history.

## Submission Instructions
The project archive (and repo as well) must contain at least the following mandatory files:

* all project source codes and stand-alone libraries;
* working `Makefile`, which will compile or build the executable binary (with the filename specified in the assignment) from the source codes;
* documentation written in Markdown in the file `README.md`;
* the license in the file `LICENSE` with the most restrictive (i.e., preserving maximum rights of you as the author) commonly recognized open-source license you can find;
* `CHANGELOG.md` will briefly describe implemented functionality and known limitations (a documented problem is better than an undocumented one).
If you are unaware of any limitations, state that in the changelog as well.

All of the aforementioned files are expected to be found **in the root directory** of the project.
Do not submit any binary or build-generated files.

Please verify the following steps for the successful submission of your projects:

1) check whether your code successfully compiles/builds <ins>under the reference developer environment</ins> and check the filename syntax of mandatory files;
2) upload ZIP (and only ZIP) archive with a filename matching `xlogin99.zip`, containing the contents of your Gitea repo with all mandatory files (see above);
3) give access to your private Gitea repo to logins `veselyv`, `dolejska` and `izavrel`.

We will ignore any submission via email (or any other communication channel) after the deadline.

## Documentation Instructions
Good documentation should include the following:
* content structuring;
* executive summary of the theory necessary to understand the functionality of the implemented application;
* UML diagrams or at least a narrative about interesting source code sections;
* most importantly, the testing part, which should prove that you have done some validation and verification of implemented functionality;
* description of extra functionality or features beyond the standard outlined by the assignment;
* bibliography with the list of all the sources used.

Documentation can be in the following languages:
* Czech,
* Slovak,
* or English.

Source code comments must be in the following language: English.

The section of the documentation describing testing should be sound and reproducible (e.g., an accurate description of network topology, involved hardware specification and software versions).
Testing should include at least a few runs of your application testing proper functionality and various edge cases.
Please, compare your project with another similar tool (if such tool exists).
Since it is your application, you should also be the author of test sets (i.e., avoid using someone else's tests).
Testing proofs can have textual program outputs (screenshots for textual information are undesired).
Presenting testing inputs/outputs is not enough; a good testing section should describe the following:
* what was tested
* why it was tested
* how it was tested
* what was the testing environment
* what were the inputs, expected outputs, and actual outputs

When working with any bibliography or sources online, please cite according to [faculty guidelines](https://www.fit.vut.cz/study/theses/citations/.en).
It is also worth reading this [web page](http://www.fit.vutbr.cz/~martinek/latex/citace.html) by David Martinek.

When adopting or inserting snippets of someone else's source code into your project, you need to use them according to the author's license terms.
Moreover, you need to mark this source code and reference the author explicitly in your source files.
Avoid any unidirectional or bidirectional plagiarism!

## Developer Environment and Virtualization Remarks
The individual assignment specifies a custom reference environment located in [its own repository](https://git.fit.vutbr.cz/NESFIT/dev-envs).
The reference environment is separate from the provided virtual machine and can be activated on-demand for a particular language, mainly to avoid wasting resources with unnecessary language toolchains.

To utilize the developer environment, you can use the virtual machine(s) downloadable from
[FIT Nextcloud](https://nextcloud.fit.vutbr.cz/s/N5fM3Njwm6yfbeZ)
(hosted by, e.g.,
[VMWare Player](https://www.vmware.com/products/workstation-player.html)
or [VirtualBox](https://www.virtualbox.org/)
or [Hyper-V](https://learn.microsoft.com/en-us/virtualization/hyper-v-on-windows/about/)
or [Parallels](https://www.parallels.com/) depending on your platform).
The user login is `ipk` with password `ipk`.
Alternatively, you can install [Nix, the package manager](https://nixos.org/download.html#nix-install-linux) on your own machine (easy instalation can be achieved following the [Zero to Nix guide](https://zero-to-nix.com/start/install)).

> <span style="color:orange">Projects will be evaluated on the reference virtual machine and corresponding architecture.
In case you do not have access to the corresponding architecture/platform you can always leverage services of [CVT at FIT](https://www.fit.vut.cz/units/cvt/.cs).
</span>

These are the developer environments acceptable for this year:
* `c`
* `csharp`

To activate or modify the environment, follow the guide in the corresponding [developer environment repository](https://git.fit.vutbr.cz/NESFIT/dev-envs#starting-development-environment).
However, your *Makefile* should work with the default, unedited environment.

Due to the nature of the projects, you can assume that:
* the project will be run and evaluated with root privileges,
* the reference machine will be connected to the Internet.

## Evaluation
You can get up to 14 points for working applications in compliance with the assignment, as well as demonstrating good programming skills.
You can get up to 6 points for documentation.
You can earn extra points for significant extension of your program's features.
You cannot get more than 20 points from the project, but the points for extra effort can help you minimize any losses during the assesment of standard requirements.

Common criteria for the evaluation:
- code and application behaviour:
  * unstructured, uncommented, single-file or spaghetti source code = up to -3 points
  * non-functioning or missing Makefile = 0 points (-4 if fixed)
  * wrong filenames or location of mandatory files = up to -2 points
  * temporary or irrelevant files inside the submitted archive = up to -2 points
  * incorrect dynamic memory handling (e.g., memory leaks, unclosed file descriptors) = up to -4
  * input/output of the program is not in line with the assignment requirements = up to -10
- documentation:
  * missing or poor documentation = up to -6 points
  * wrong bibliography or poor formatting = up to -1,5 points
  * insufficient proof of testing of the project = up to -3 points
- general criteria:
  * the project cannot be compiled, executed or tested = 0 points
  * the program ends abruptly (e.g., `SEGFAULT`, `SIGSEGV`) = 0 points
  * the project solution is not in compliance with the assignment = 0 points
  * plagiarism = 0 points for all involved parties and personal meeting with teacher, guarantor and expel committee

The archive submitted via IS VUT will be the basis for your evaluation.
Files in the Gitea repo are additional content accompanying the submission but take no precedence over the content in IS VUT.
Therefore, any technical difficulties on the Gitea side will not affect your score.

----

## Project 1 - Client for a chat server
### Deadlines
Monday 1st April 2024 at 22:22:00

### Languages
* C
* C++
  * Object-oriented programming is mandatory
* C#
  * Object-oriented programming is mandatory

### Variants
* Last year's points from Project 1
* [Client for a chat server](https://git.fit.vutbr.cz/NESFIT/IPK-Projects-2024/src/branch/master/Project%201)

## Project 2 - Multiple variants
### Deadlines
Monday 22nd April 2024 at 22:22:00 

### Languages
* C
* C++
  * Object-oriented programming is mandatory
* C#
  * Object-oriented programming is mandatory

### Variants
* ALPHA: Last year's points from Project 2
* [DELTA: L2/L3 scanner](https://git.fit.vutbr.cz/NESFIT/IPK-Projects-2024/src/branch/master/Project%202/delta)
* [OMEGA: L4 scanner](https://git.fit.vutbr.cz/NESFIT/IPK-Projects-2024/src/branch/master/Project%202/omega)
* [ZETA: Network sniffer](https://git.fit.vutbr.cz/NESFIT/IPK-Projects-2024/src/branch/master/Project%202/zeta)
* [IOTA: Chat server](https://git.fit.vutbr.cz/NESFIT/IPK-Projects-2024/src/branch/master/Project%202/iota)

Students of 2BIT (i.e., students in the second year of the BIT bachelor's program) can choose from _any non-ALPHA_ variant.

> <span style="color:red">Students of 3BIT (i.e., all non-2BIT students such as students of 3rd and later years) can register <i>only for the ALPHA or IOTA</i> variant!
Submissions of students not following this restriction <b>will not be evaluated</b>!
</span>
The reason behind this restriction is that no student of the IPK course should have the chance to repeat the same project twice.
