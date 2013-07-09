drone_simulator
===============

Undocumented in dev guide:
- SDK first connects with FTP to get version.txt. Looks like this is initiated on 5551 but transfer occurs elsewhere? This may just be how FTP works; need to look into the spec more.
- Then it connects to port 5552 for the authentication method
