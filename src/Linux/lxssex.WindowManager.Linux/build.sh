export DOTNET_ROOT=$HOME/dotnet
export PATH=$PATH:$HOME/dotnet
dotnet publish /p:NativeLib=Shared -r $1 -c $2