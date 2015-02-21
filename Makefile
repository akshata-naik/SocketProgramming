all: server_out doctor_out patient1_out patient2_out

server_out:healthcenterserver.c
	gcc -o server_out healthcenterserver.c -lnsl -lsocket -lresolv

doctor_out:doctor.c
	gcc -o doctor_out doctor.c -lnsl -lsocket -lresolv

patient1_out:patient1.c
	gcc -o patient1_out patient1.c -lnsl -lsocket -lresolv
	
patient2_out:patient2.c
	gcc -o patient2_out patient2.c -lnsl -lsocket -lresolv

