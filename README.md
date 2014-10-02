PPM
=======

PPM stands for Personal Password Manager.
PPM is a simple secure password manager, passwords are encrypted and decrypted using a key provided by the user.

Requirements
-------
For encryption PPM requires the OpenSSL development libraries which can be downloaded here: https://www.openssl.org/source
Optionally readline libraries can be installed for commandline completion and history in interactive mode.


Installation
-------

```
git clone https://github.com/niels07/ppm.git
cd ppm
./configure
make
make install
```

Usage
-------
You can run PPM interactively or straight from the commandline.
To run a single command:

```
ppm -k <KEY> [COMMMAND] [PARAMS]
```

To run interactively:
```
ppm -k <KEY>
```

or just
```
ppm
```

To get a list of available commands run
```
ppm help
```

When interactive, to save your changes, you'll need to use the 'save' command before exiting, unless -s is specified.

A simple example to add a user to a file "test.txt" with key "ppm" with username "niels" and password "test"

```
ppm -f ./test.txt -k ppm add niels test
```

Then to access the user you'd run:
```
ppm -f ./test.txt -k ppm get niels
```

Which will print the password "test"


