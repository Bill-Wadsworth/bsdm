# BSDM 
BS-DisplayManager

this is a WIP display manager

## Currently working:
- can log you in 
- Boots an X server (if you set /etc/X11/Xwrapper.config to have line: allowed_users=anybody)
- looks kinda alright

# TODO:

## AUTH
- [x] somewhat working auth
- [ ] better logging
- [ ] remove the requirements for the Xwrapper config
- [ ] reduce places in auth where curruption is likely. (strdup and such)

## UI
- [x] get username and password
- [ ] switch between different launch commands
- [ ] get the host name of the machine
- [ ] get the OS and display correct ascii art
