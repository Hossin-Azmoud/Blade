set -xe
make
sudo mv ./bin/blade /usr/local/bin/blade
echo "blade was installed successfully (Into </usr/local/bin>), Run the \`blade\` Command to Start using the editor."

