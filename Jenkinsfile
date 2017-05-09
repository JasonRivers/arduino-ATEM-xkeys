node('arduino') {
	stage ('Checkout') {
		checkout scm
		sh 'git submodule update --init'
	}
	stage ('build') {
		sh 'mkdir -p build && arduino-builder -build-options-file ./build.options.json -verbose -build-path ${WORKSPACE}/build/ -compile ./arduino-ATEM-xkeys.ino'
	}
}
