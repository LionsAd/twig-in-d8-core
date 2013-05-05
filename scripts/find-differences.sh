#!/bin/bash

#git remote prune origin
#git fetch origin
diff -u <(git branch -a | grep issue- | grep 'origin' | sed 's,remotes/origin/,,' | sort) <(git branch -a | grep issue- | grep -v 'origin' | sort) | grep -- '^-' | grep 'issue-' | cut -d'-' -f3 | sed 's/^/#/'
