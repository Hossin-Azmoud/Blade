set -xe
make
sudo mv ./bin/mi /usr/local/bin/mi
echo "Mi was installed successfully (Into </usr/local/bin>), Run the \`mi\` Command to Start using the editor."

