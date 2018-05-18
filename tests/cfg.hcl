global {
	log_directory = "/var/log/kaohi"
	log_level = 1

	listen_address = "127.0.0.1:8443"
}

config-files "group1" {
	files = [
		"/var/log/system.log",
		"/var/log/fail.log"
	]
}

config-files "group2" {
	files = [
		"/var/log/openvpn.log",
	]
}

commands "group1" {
	uid = 501
	interval = 10
	cmds = [
		"/bin/ls -l /",
		"tcpdump -i eth0"
	]
}

rsyslog {
	listen_address = "*.5080"
	protocol = "tcp"
}
