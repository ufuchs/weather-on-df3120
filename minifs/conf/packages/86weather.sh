
PACKAGES+=" sdlweather"
hset sdlweather url "none" 
hset sdlweather depends "libsdl libsdlimage libsdlttf"

install-sdlweather() {
	cp sdlweather "$STAGING_USR"/bin/sdlweather
	log_install echo Done
}

deploy-sdlweather() {
	deploy cp "$STAGING_USR"/bin/sdlweather "$ROOTFS"/usr/bin
}

