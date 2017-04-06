# quickbox_themes
All additional themes for the QuickBox Dashboard are housed here... nice and tidy like! Want to make your own? Fork and submit!

### Want to install the QuickBox Smoked Theme?

#### First you'll need to navigate to your dashboard directory

```
cd /srv/rutorrent/home
```

#### Next, you'll need to initialize the directory with git

```
git init
git remote add -t \* -f origin https://github.com/QuickBox/quickbox_themes.git
```

You'll now see the themes which are organized into separate origins

> #### Heads Up!
origin/master is not a theme and only contains basic readme intros for the themes to be added

```
Updating origin
remote: Counting objects: 444, done.
remote: Compressing objects: 100% (397/397), done.
remote: Total 444 (delta 43), reused 430 (delta 33), pack-reused 0
Receiving objects: 100% (444/444), 2.13 MiB | 3.93 MiB/s, done.
Resolving deltas: 100% (43/43), done.
From https://github.com/QuickBox/quickbox_themes
 * [new branch]      defaulted  -> origin/defaulted
 * [new branch]      master     -> origin/master
 * [new branch]      smoked     -> origin/smoked
```

#### Now fetch the themes repository

```
git fetch --all
```

#### Finally, you can use the theme with the following

**For the 'Smoked' theme**
```
git reset --hard origin/smoked
```
**For the 'Defaulted' theme** (QuickBox default dashboard theme)
```
git reset --hard origin/defaulted
```

> You may need to restart apache to see these changes go into effect immediately. You can do so with `service apache2 restart`

---

### Need to update the theme?

**For the 'Smoked' theme**
```
cd /srv/rutorrent/home
git pull origin smoked
```

**For the 'Defaulted' theme**
```
cd /srv/rutorrent/home
git pull origin defaulted
```

Simple as that! You will then see output such as:
```
From https://github.com/QuickBox/quickbox_themes
 * branch            smoked     -> FETCH_HEAD
Updating d8b7dfd..d2efa50
Fast-forward
 README.md | 42 ++++++++++++++++++++++++++++++++++++++++++
 1 file changed, 42 insertions(+)
```
