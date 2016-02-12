# Home

Git Instructions

Fetching latest version 
$ git fetch

$ git rebase

If rebase gives an error, it's likely since some files are changed after the github, to resolve:

$ git stash

$ git rebase

$ git stash pop

Commiting files
$ git add [your file]

$ git status

Check that everything looks OK, and then

$ git commit -m "your message"

$ git push

Depending on your git settings, you might be prompted for github username & password
