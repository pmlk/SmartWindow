
sudo apt-get install mysql-server
wget http://downloads.sourceforge.net/project/libcoap/coap-18/libcoap-4.0.3.tar.gz
tar -xzf libcoap-4.0.3.tar.gz
rm libcoap-4.0.3.tar.gz
cd libcoap-4.0.3
# autoconf # auskommentiert da es nicht funktioniert
./configure
make
sudo cp examples/coap-client /usr/local/bin/  or copy to ~/bin and set it to your syspath
