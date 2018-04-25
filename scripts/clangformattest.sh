#!/bin/bash

current_dir=`dirname "${BASH_SOURCE[0]}"`

initial_state=`git status | grep "modified"`
if ! [[ -z $initial_state ]]; then
    echo "Clang-format script must be applied on a clean git state"
    exit 1
fi

"${current_dir}"/run-clang-format.sh && exit 0

final_filelist=`git status | grep "modified"`
if [[ -z $final_filelist ]]; then
    echo "Code-style is pretty good :thumbsup:"
    exit 0;
fi

echo "Read code-style guide again and apply those patches:"
git diff

git reset HEAD --hard
exit 1
