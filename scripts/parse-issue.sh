#!/bin/bash

FILE=$1

PATCH=$(grep 'href=' "$FILE" | grep files | sed 's/href="/|/g' | cut -d '|' -f2 | cut -d'"' -f1 | egrep '\.patch|\.diff' | tail -n 1)
TITLE=$(grep '<title>' "$FILE" | cut -d'>' -f2 | cut -d'|' -f1)
STATUS=$(grep 'Status:' "$FILE" | grep Status | head -n1 | cut -d'>' -f5 | cut -d '<' -f1)

case $TITLE in *"[meta]"*) exit 1 ;; esac
case $STATUS in "needs review"|"reviewed &amp; tested"*) true ;; *) exit 1 ;; esac

[ -n "$PATCH" ] && echo "$PATCH|$STATUS|$TITLE"
