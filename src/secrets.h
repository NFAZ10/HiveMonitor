#ifndef SECRETS_H
#define SECRETS_H



#define SECRET
//#define THINGNAME "HiveMonitor001"                         //change this
 
const char SSID[] = "FazioIOT";               //change this
const char PASS[] = "824656789NAF";           //change this
const char AWS_IOT_ENDPOINT[] = "a36a2to8608hle-ats.iot.us-east-2.amazonaws.com";       //change this
 
// Amazon Root CA 1
static const char AWS_CERT_CA[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
MIIDQTCCAimgAwIBAgITBmyfz5m/jAo54vB4ikPmljZbyjANBgkqhkiG9w0BAQsF
ADA5MQswCQYDVQQGEwJVUzEPMA0GA1UEChMGQW1hem9uMRkwFwYDVQQDExBBbWF6
b24gUm9vdCBDQSAxMB4XDTE1MDUyNjAwMDAwMFoXDTM4MDExNzAwMDAwMFowOTEL
MAkGA1UEBhMCVVMxDzANBgNVBAoTBkFtYXpvbjEZMBcGA1UEAxMQQW1hem9uIFJv
b3QgQ0EgMTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBALJ4gHHKeNXj
ca9HgFB0fW7Y14h29Jlo91ghYPl0hAEvrAIthtOgQ3pOsqTQNroBvo3bSMgHFzZM
9O6II8c+6zf1tRn4SWiw3te5djgdYZ6k/oI2peVKVuRF4fn9tBb6dNqcmzU5L/qw
IFAGbHrQgLKm+a/sRxmPUDgH3KKHOVj4utWp+UhnMJbulHheb4mjUcAwhmahRWa6
VOujw5H5SNz/0egwLX0tdHA114gk957EWW67c4cX8jJGKLhD+rcdqsq08p8kDi1L
93FcXmn/6pUCyziKrlA4b9v7LWIbxcceVOF34GfID5yHI9Y/QCB/IIDEgEw+OyQm
jgSubJrIqg0CAwEAAaNCMEAwDwYDVR0TAQH/BAUwAwEB/zAOBgNVHQ8BAf8EBAMC
AYYwHQYDVR0OBBYEFIQYzIU07LwMlJQuCFmcx7IQTgoIMA0GCSqGSIb3DQEBCwUA
A4IBAQCY8jdaQZChGsV2USggNiMOruYou6r4lK5IpDB/G/wkjUu0yKGX9rbxenDI
U5PMCCjjmCXPI6T53iHTfIUJrU6adTrCC2qJeHZERxhlbI1Bjjt/msv0tadQ1wUs
N+gDS63pYaACbvXy8MWy7Vu33PqUXHeeE6V/Uq2V8viTO96LXFvKWlJbYK8U90vv
o/ufQJVtMVT8QtPHRh8jrdkPSHCa2XV4cdFyQzR1bldZwgJcJmApzyMZFo6IQ6XU
5MsI+yMRQ+hDKXJioaldXgjUkK642M4UwtBV8ob2xJNDd2ZhwLnoQdeXeGADbkpy
rqXRfboQnoZsG4q5WTP468SQvvG5
-----END CERTIFICATE-----
)EOF";
 
// Device Certificate                                               //change this
static const char AWS_CERT_CRT[] PROGMEM = R"KEY(
-----BEGIN CERTIFICATE-----
MIIDWTCCAkGgAwIBAgIUZIPDe3nB1Jnbue+VAZFSt8nAR7UwDQYJKoZIhvcNAQEL
BQAwTTFLMEkGA1UECwxCQW1hem9uIFdlYiBTZXJ2aWNlcyBPPUFtYXpvbi5jb20g
SW5jLiBMPVNlYXR0bGUgU1Q9V2FzaGluZ3RvbiBDPVVTMB4XDTI1MDEwNzE2NDc0
NloXDTQ5MTIzMTIzNTk1OVowHjEcMBoGA1UEAwwTQVdTIElvVCBDZXJ0aWZpY2F0
ZTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAKcypeb6e0al7Ntd5uvN
ZTjBrVQg7yzXSawqSosDhRqBbCi/hSfJ0NSKCAhKjtwPr1xNvpISj6LSXWcgl50Q
nW0dH4bN0ebGRw9P5bHygjME4p7nYTq5/R7a/vsApHFIRSTo7Rz9rD/ED1XTsSUK
E+jAf51huUOnT5pBDZk1jx8TYVeCkhxCcXRfbGH73sf1rOkJk8t2zVybnXtjfnS+
8KMcPY4olc+gjv0J2er1JqVCWGB8BJCD6ZOROUIP6Bo+JewCnkPdqVzOr2LVznud
G2iqv2dl5rn80pZ0PuSUWgXajEEp3kl/wxq6N4WJ3fDaR3N1b1j8TO1tdn0flYod
870CAwEAAaNgMF4wHwYDVR0jBBgwFoAU3ayZf9AsNTcy8Qhj3obsD/cjXVYwHQYD
VR0OBBYEFFQa8yhQmPoelFwHxz1P+XzgSrf1MAwGA1UdEwEB/wQCMAAwDgYDVR0P
AQH/BAQDAgeAMA0GCSqGSIb3DQEBCwUAA4IBAQBVPfNlX9yoxtH/cLEiF/3Ed8wb
UvtmsmHG2Ft1CrRpd4Gv1QFP2KO5cSJwoW19vm+X9Dlsl0+iw8T09SjN0HFDY47k
8O7PJ5ACElrH1wVTBhRfeCJLLILWFzbvTj5lbbSVPTDgQ49mhXoVJimoxuLclHPt
PtVSxoM7tx1dxa4G8F8V5eQZyaaSuCH2pRkV35DhulPtwUcWIkyDPQHvgarzqJP7
aP1nGFRO4frtqFckZtAUkbTc+upuWWsqx7rLH+p9unv8RhQw1UEuCp/WuCvj+YZU
dCyyOv+OtrxmeZQj7h19/mm8UyHHr7SaA+uiUk+QaCcq4ooZr4tITLu0JWkO
-----END CERTIFICATE-----
)KEY";
 
// Device Private Key                                               //change this
static const char AWS_CERT_PRIVATE[] PROGMEM = R"KEY(
-----BEGIN RSA PRIVATE KEY-----
MIIEpAIBAAKCAQEApzKl5vp7RqXs213m681lOMGtVCDvLNdJrCpKiwOFGoFsKL+F
J8nQ1IoICEqO3A+vXE2+khKPotJdZyCXnRCdbR0fhs3R5sZHD0/lsfKCMwTinudh
Orn9Htr++wCkcUhFJOjtHP2sP8QPVdOxJQoT6MB/nWG5Q6dPmkENmTWPHxNhV4KS
HEJxdF9sYfvex/Ws6QmTy3bNXJude2N+dL7woxw9jiiVz6CO/QnZ6vUmpUJYYHwE
kIPpk5E5Qg/oGj4l7AKeQ92pXM6vYtXOe50baKq/Z2XmufzSlnQ+5JRaBdqMQSne
SX/DGro3hYnd8NpHc3VvWPxM7W12fR+Vih3zvQIDAQABAoIBAEZ/8rhsUeK+QBtX
gvFW0CGYeO+KW496z9PKmVf4if1dKTAdYZYfYPDdqvlkBsz/yB+RoJOTRAGR1lSP
x54wzVCfARYkXyeB36CzPdVEvmWo363FPpg2qGaETROld7FUmeMwXBrnU+QnVZgJ
BR7rGu3/jU/dTvMZ71TJrWQ20zx0jvNVYDzIgWKzwtP3xZJq+thh86DpdT4o2miE
/rbgo37HewTn0kgRkam7xdjWtRk09xJpLo9OBQdDSmsdUIDDJXWSijmBDZfQYZlG
ioiBTUOcF7/6A9xj1zko3eIF4te/y6kHvNLv9rTOx4LHDjUWPzR03DJ24fvOu6e5
nh2bcnkCgYEA1u9g4k7nz2XidNwc41+TujWv6R7RXxyoPN/2NVSghvTQbjDhl2Tk
0McPo0xy5UXdUozUSwNSFRK7pjRp8hPCrwU/6yEDUUVOF8plCfhyuWTqrr7OhAbC
BVVBHy7AjPz7RtgfFadHsqLBJwc53nEWDO5P8DLD2EJcbjjdw2Gq+KcCgYEAxyRo
ugcw9lQt8yJjoKgFVXqOC4MBAzx3cGFUNLo4qdRvoFZEF6vlus7ktNNYYgUD0qPV
EnZhpcOWXDT8STvA1LOWJFwXn84mjVDPcmyQI+aTJA5/OgcSOrCR8KQipcvC35LF
xMYLbxVfR6ey1dQZ75r0BWiiD4SdT99qnu9/mPsCgYBPBMabrOKwbwaoa9v7A8Jt
7Rr1G61hyElZyATeanRx6UNPn4zwedldoDFxmcOL4vmRRdEumtPsh2yRmCdzJiym
58kuCn8Lu+0P9grbdYnH+wAEqoWGKG3gRA2jBy8nJkLDMuWKsms7k3ZrgM6Xpv+y
VVLyPdeehBczNpjhIeEXdQKBgQCCyQ2ihxCH5ncbA83MeCywm8FoHAxxNKf4fWNR
EbAfES6SbbVgmntQuYoIm/iiv0pob0KDUo4E0LQSMJYz/nNA/gSwMm8te7Vj/9uN
+XCcs1SVTJQP5w74zne6RAGhQ3XjYUBTYAGoXa/qnl9cAWVbMDfsyXANovmxuDgM
05vIOwKBgQCOJvSRtkfK+kSQgpUCgQB27r9c17CetAI9V9qI3xYVG/fcmfgCjynW
ruR1qJafInZTvhAMlsd1ZLUyeg8W7eL1TO7U+GFL9DKecIpSoFxc+HOOdmGSiRl+
i727lpWf9A6yOtOHsFJbfWl9/CZnqRgDqWDeObxwz0BE7Fg9ozcPUQ==
-----END RSA PRIVATE KEY-----
)KEY";


#endif