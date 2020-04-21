add-type @"
using System.Net;
using System.Security.Cryptography.X509Certificates;
public class TrustAllCertsPolicy : ICertificatePolicy {
    public bool CheckValidationResult(
        ServicePoint srvPoint, X509Certificate certificate,
        WebRequest request, int certificateProblem) {
        return true;
    }
}
"@
$AllProtocols = [System.Net.SecurityProtocolType]'Ssl3,Tls,Tls11,Tls12'
[System.Net.ServicePointManager]::SecurityProtocol = $AllProtocols
[System.Net.ServicePointManager]::CertificatePolicy = New-Object TrustAllCertsPolicy

#Download GLFW Version 3.3
$file = "GLFW.zip"

Invoke-WebRequest -Uri "https://github.com/glfw/glfw/releases/download/3.3/glfw-3.3.zip" -OutFile $file -TimeoutSec 5

# Unzip the file to specified location
$location = Get-Location
$zip_file = (new-object -com shell.application).namespace("$location\$file") 
$destination = (new-object -com shell.application).namespace("$location")
$items = $zip_file.items()
$destination.Copyhere($items)
rename-item "glfw-3.3" "GLFW"
remove-item GLFW.zip

# Download libmorton v0.2.2
$file = "v0.2.2.zip"

Invoke-WebRequest -Uri "https://github.com/Forceflow/libmorton/archive/v0.2.2.zip" -OutFile $file -TimeoutSec 5

# Unzip the file to specified location
$location = Get-Location
$zip_file = (new-object -com shell.application).namespace("$location\$file")
$destination = (new-object -com shell.application).namespace("$location")
$items = $zip_file.items()
$destination.Copyhere($items)
rename-item "libmorton-0.2.2" "libmorton"


remove-item "v0.2.2.zip"