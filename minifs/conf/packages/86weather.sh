
PACKAGES+=" sdlweather"
hset sdlweather url "none" 
hset sdlweather depends "libsdl libsdlimage libsdlttf"

install-sdlweather() {
	cp sdlweather "$STAGING_USR"/bin/sdlweather
	cp -r {fonts,icons} "$STAGING_USR"/share
	log_install echo Done
}

deploy-sdlweather() {
	deploy cp "$STAGING_USR"/bin/sdlweather "$ROOTFS"/usr/bin
	deploy cp -r "$STAGING_USR"/share/fonts "$ROOTFS"/usr/share
	deploy cp -r "$STAGING_USR"/share/icons "$ROOTFS"/usr/share
}

