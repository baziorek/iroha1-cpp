.. contents:: **Table of Contents**
  :depth: 3

First off, thanks for taking the time to contribute!

The following is a short set of guidelines for contributing to Iroha.

How Can I Contribute?
---------------------

Translating Documentation
~~~~~~~~~~~~~~~~~~~~~~~~~

Translated versions of this documentation are hosted next to the English
one under ``/<language>/``. Currently available:

* **Türkçe** — `Türkçe dokümantasyon <../tr/>`_ (translated from the
  archived :strikelink:`hyperledger-iroha/iroha-docs-l10n
  <https://github.com/hyperledger-iroha/iroha-docs-l10n>` repository;
  the translation covers Iroha 1.2-era docs and may lag behind the
  English version)

More languages are planned — including Polish. If you want to
contribute a translation, please get in touch via
`Discord <https://discord.com/channels/905194001349627914/905205848547155968>`_.

Reporting Bugs
~~~~~~~~~~~~~~

*Bug* is an error, design flaw, failure or fault in Iroha that causes it
to produce an incorrect or unexpected result, or to behave in unintended
ways.

Bugs are tracked as `GitHub issues <https://github.com/hyperledger-iroha/iroha/issues>`_.

To submit a bug, `create new issue <https://github.com/hyperledger-iroha/iroha/issues/new>`_ and
include these details in the issue template: a summary of the problem,
reproduction steps, and any relevant logs. Please also provide your
environment details (OS, device specs, Iroha version etc.).

Reporting Vulnerabilities
~~~~~~~~~~~~~~~~~~~~~~~~~

While we try to be proactive in preventing security problems, we do not
assume they'll never come up.

It is standard practice to responsibly and privately disclose to the
vendor (Hyperledger organization) a security problem before publicizing,
so a fix can be prepared, and damage from the vulnerability minimized.

Before the First Major Release (1.0) all vulnerabilities are considered
to be bugs, so feel free to submit them as described above. After the
First Major Release please utilize `a bug bounty program
here <https://hackerone.com/hyperledger>`__ in order to submit
vulnerabilities and get your reward.

In any case ? feel free to reach to any of existing maintainers in
Discord private messages or in an e-mail (check CONTRIBUTORS.md
file) if you want to discuss whether your discovery is a vulnerability
or a bug.

Suggesting Improvements
~~~~~~~~~~~~~~~~~~~~~~~

An *improvement* is a code or idea, which makes **existing** code or
design faster, more stable, portable, secure or better in any other way.

Improvements are tracked as `GitHub issues <https://github.com/hyperledger-iroha/iroha/issues>`_.

To submit a new improvement, `create new issue <https://github.com/hyperledger-iroha/iroha/issues/new>`_ and
describe the idea: what it improves and why it is valuable. If you
have any code suggestions, you are welcome to include them.

Asking Questions
~~~~~~~~~~~~~~~~

A *question* is any discussion that is typically neigher a bug, nor
feature request or improvement. If you have a question like "How do I do
X?" - this paragraph is for you.

Please post your question in `your favourite
messenger <#places-where-community-is-active>`__ so members of the
community could help you. You can also help others!

Your First Code Contribution
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Read our `C++ Style Guide <#c-style-guide>`__ and start with checking out `the GitHub board <https://github.com/hyperledger-iroha/iroha/projects/1>`_ or issues with the
`good-first-issue label <https://github.com/hyperledger-iroha/iroha/issues?q=is%3Aissue+is%3Aopen+label%3Agood-first-issue>`_.
Indicate somehow that you are working on this task: get in touch with
maintainers team, community or simply assign this issue to yourself.

Pull Requests
~~~~~~~~~~~~~

-  Fill in `the required template <https://github.com/hyperledger-iroha/iroha/blob/iroha1-main/.github/PULL_REQUEST_TEMPLATE.md>`_

-  End all files with a newline

-  **Write tests** for new code. Test coverage for new code must be at
   least 70%

-  Every pull request should be reviewed and **get at least two
   approvals from maintainers team**. Check who is a current maintainer
   in
   `MAINTAINERS.md <https://github.com/hyperledger-iroha/iroha/blob/iroha1-main/MAINTAINERS.md>`_
   file

-  When you've finished work make sure that you've got all passing CI
   checks ? after that **squash and merge** your pull request

-  Follow the `C++ Style Guide <#c-style-guide>`_

-  Follow the `Git Style Guide <#git-style-guide>`_

-  **Document new code** based on the `Documentation
   Styleguide <#documentation-styleguide>`__

-  When working with **PRs from forks** check `this
   manual <https://help.github.com/articles/checking-out-pull-requests-locally>`_

Styleguides
-----------

Git Style Guide
~~~~~~~~~~~~~~~

-  **Sign-off every commit** with `DCO <https://github.com/apps/dco>`_:
   ``Signed-off-by: $NAME <$EMAIL>``. You can do it automatically using
   ``git commit -s``
-  **Use present tense** ("Add feature", not "Added feature").
-  **Use imperative mood** ("Deploy docker to..." not "Deploys docker
   to...").
-  Write meaningful commit message.
-  Limit the first line of commit message to 50 characters or less
-  First line of commit message must contain summary of work done,
   second line must contain empty line, third and other lines can
   contain list of commit changes
-  When merging to destination branche: use **Squash and merge**

C++ Style Guide
~~~~~~~~~~~~~~~

-  Use clang-format
   `settings <https://github.com/hyperledger-iroha/iroha/blob/iroha1-main/.clang-format>`_
   file. There are guides available on the internet (e.g. `Kratos
   wiki <https://github.com/KratosMultiphysics/Kratos/wiki/How-to-configure-clang%E2%80%90format>`_)
-  Follow
   `CppCoreGuidelines <http://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines>`_
   and `Cpp Best
   Practices <https://lefticus.gitbooks.io/cpp-best-practices>`_.
-  Avoid
   `platform-dependent <https://stackoverflow.com/questions/1558194/learning-and-cross-platform-development-c>`_
   code.
-  Use `C++23 <https://en.wikipedia.org/wiki/C%2B%2B23>`_.
-  Use `camelCase <https://en.wikipedia.org/wiki/Camel_case>`_ for
   class names and methods, use
   `snake\_case <https://en.wikipedia.org/wiki/Snake_case>`_ for
   variables.

Documentation Styleguide
~~~~~~~~~~~~~~~~~~~~~~~~

-  Use
   `Doxygen <http://www.doxygen.nl/>`_.
-  Document all public API: methods, functions, members, templates,
   classes...

Places where community is active
--------------------------------

Our community members are active at:

+----------------+-------------------------------------------------------------------------------------------------------+
| Service        | Link                                                                                                  |
+----------------+-------------------------------------------------------------------------------------------------------+
| Discord        | https://discord.com/channels/905194001349627914/905205848547155968 **(recommended)**                  |
+----------------+-------------------------------------------------------------------------------------------------------+
| RocketChat     | :strikelink:`Rocket.Chat <https://chat.hyperledger.org/channel/iroha>`                                |
+----------------+-------------------------------------------------------------------------------------------------------+
| Wiki (Iroha 1) | :strikelink:`Iroha 1 <https://lf-hyperledger.atlassian.net/wiki/spaces/iroha/pages/21015959/Iroha+1>` |
+----------------+-------------------------------------------------------------------------------------------------------+
| StackOverflow  | https://stackoverflow.com/questions/tagged/hyperledger-iroha                                          |
+----------------+-------------------------------------------------------------------------------------------------------+
| Mailing List   | hyperledger-iroha@lists.hyperledger.org                                                               |
+----------------+-------------------------------------------------------------------------------------------------------+
| Gitter         | https://gitter.im/hyperledger-iroha/Lobby (defunct since 2023)                                        |
+----------------+-------------------------------------------------------------------------------------------------------+
| Telegram       | https://t.me/hl\_iroha                                                                                |
+----------------+-------------------------------------------------------------------------------------------------------+
| YouTube        | https://www.youtube.com/channel/UCYlK9OrZo9hvNYFuf0vrwww                                              |
+----------------+-------------------------------------------------------------------------------------------------------+


--------------

Thank you for reading the document!
