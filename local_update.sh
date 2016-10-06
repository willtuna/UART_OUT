#! /bin/bash
echo "Calling git update shell ... made by Vegapunk"

git status

git add -A
git commit
git push orgin master
echo "Update Done"