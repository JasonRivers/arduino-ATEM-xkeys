node('arduino') {
	stage ('Checkout') {
		checkout scm
	}
	stage ('build') {
		sh 'arduino-builder -build-options-file ./build.options.json -verbose -compile ./arduino-ATEM-xkeys.ino'
	}
}
