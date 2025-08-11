# BSDM 
BS-DisplayManager

this is a WIP display manager

## Currently working:
- Auth 
- Boots an X server (if you set /etc/X11/Xwrapper.config to have line: allowed_users=anybody)
- The worlds worst UI (does not hide password)

## TODO:
- [ ] Improve the UI to make it a nicer experience
- [ ] better logging?
- [ ] remove the requirements for the Xwrapper config
- [ ] reduce places in auth where curruption is likely. (strdup and such)
- [ ] add a make install and other related commands
- [ ] finish the TODO list
