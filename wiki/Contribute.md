Here are some tips when you want to contribute to PVSnesLib and commit code to be reviewed

# Master branch is forbidden !

Do not push your pull request on master branch, always use the develop one

# Semantic Commit Messages

See how a minor change to your commit message style can make you a better programmer.

Format: `<type>(<scope>): <subject>`

`<scope>` is optional

### Example

```
feat: add hat wobble
^--^  ^------------^
|     |
|     +-> Summary in present tense.
|
+-------> Type: chore, docs, feat, fix, refactor, style, or test.
```

More Examples:

- `feat`: (new feature for the user, not a new feature for build script)
- `fix`: (bug fix for the user, not a fix to a build script)
- `docs`: (changes to the documentation)
- `style`: (formatting, missing semi colons, etc; no production code change)
- `refactor`: (refactoring production code, eg. renaming a variable)
- `test`: (adding missing tests, refactoring tests; no production code change)
- `chore`: (updating grunt tasks etc; no production code change)

References:

- https://www.conventionalcommits.org/
- https://seesparkbox.com/foundry/semantic_commit_messages
- http://karma-runner.github.io/1.0/dev/git-commit-msg.html

# Wiki

If you want to help us to maintain the wiki or create new sections, you can do it by pull request too.

To do it, just clone the repository then update pages in the wiki folder.  
When the pull request will be approved, it will be automatically synchronized with the content available here.

The Wiki pages use the Markdown syntax, we recommend you to use Visual Studio Code to update it as you can see the result directly in the tool by using the sortcut Ctrl+Shift+V or by clicking on the icon available in the top right corner of the window.