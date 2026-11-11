# Contributing

PVSnesLib is an open-source project, and contributions can help improve its code, tools, examples and documentation.

## Before making a change

1. Read the existing documentation and check whether the topic is already covered.
2. Browse the repository to find the relevant source, example or documentation file.
3. Keep changes focused so they are easier to review.
4. Test code or documentation changes where practical, and describe what you tested.

## Contributing to the documentation

The documentation source is maintained in the repository's `docs/` directory.

For a documentation change:

1. Edit the relevant Markdown page.
2. Check links and image paths.
3. Build the site locally with the documentation dependencies installed:

   ```bash
   mkdocs build --strict
   ```

4. Review the generated site in the `site/` directory before submitting the change.

Do not duplicate the detailed function reference when a link to the generated Doxygen page is more appropriate. Likewise, supplementary community material can remain in the [Wiki](https://github.com/alekmaul/pvsneslib/wiki).

## Contributing code or examples

When changing library code, tools or examples, follow the conventions already used in the relevant part of the repository. Include a concise description of the purpose of the change and any build or test steps that were performed.

## Propose or report a change

Use the [PVSnesLib GitHub repository](https://github.com/alekmaul/pvsneslib) to review the current contribution process, report problems and submit proposed changes. Follow the instructions and templates provided by the repository at the time you contribute.

## Help improve the documentation

If you find an unclear explanation, a broken link or a missing topic, a focused correction or a report describing the problem is useful. Please include the page or file involved and, when possible, the steps needed to reproduce the issue.
