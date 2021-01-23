# Windows Build Instructions

Please note that currently only MinGW is supported.

## Symbolic links

After building will be executed command `mklink`, which must be executed via administrator.

> Creation of symbolic links requires the `SeCreateSymbolicLinkPrivilege` (“Create symbolic links”),
> which is granted only to administrators by default (but you can change that using security policy).

This command is necessary so as not to increase the size of output directories and not to make unnecessary copying of libraries.

### Steps to change the policies

1. Right click on `Start` → `Run` and launch `secpol.msc`.
2. Open `Security Settings → Local Policies → User Rights Assignment`
   and select `Create symbolic links` (that represents `SeCreateSymbolicLinkPrivilege`).
   <br>
   ![Create symbolic links](https://i.stack.imgur.com/o8J8x.png)
3. Double-click on the item and add yourself (or the whole `Users` group) to the list.

The changes will apply when you log out and log in again.

[<small>Source</small>](https://superuser.com/questions/124679/how-do-i-create-a-link-in-windows-7-home-premium-as-a-regular-user)
