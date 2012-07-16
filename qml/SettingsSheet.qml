import QtQuick 1.1
import com.nokia.meego 1.0
import com.nokia.controlpanel 0.1
import MeeGo.Connman 0.2

Sheet {
    id: sheet
    acceptButtonText: "Done"
    rejectButtonText: "Cancel"

    property QtObject network

    onNetworkChanged: {
        proxyAutoUrl.checked = ! network.proxyConfig["URL"];
    }

    onAccepted: {
        var domains = [],
            nameservers = [],
            ipv4,
            proxyconf,
            proxy_server_text;

        console.log("Accept");

        // Domains
        if (network.domains.join() !== domainsField.text) {
            if (domainsField.text) {
                domains = domainsField.text.split();
            }
            console.log("Update Domains: " + domains.join());
            network.domainsConfig = domains;
        }

        // IPv4
        ipv4 = network.ipv4;
        if (ipv4["Method"] !==  method.state) {
            ipv4["Method"] = method.state;
            if (method.state === "manual") {
                ipv4["Address"] = address.text
                ipv4["Netmask"] = netmask.text
                ipv4["Gateway"] = gateway.text
            }
            network.ipv4Config = ipv4;
        } else if (network.ipv4["Method"] === "manual") {
            if (ipv4["Address"] !== address.text ||
                ipv4["Netmask"] !== netmask.text ||
                ipv4["Gateway"] !== gateway.text) {

                ipv4["Address"] = address.text
                ipv4["Netmask"] = netmask.text
                ipv4["Gateway"] = gateway.text
                network.ipv4Config = ipv4;
            }
        }

        // Nameservers
        if (method.state === "manual" &&
            network.nameserversConfig.join() !== nameserversField.text) {
            nameservers = nameserversField.text.split();
            network.nameserversConfig = nameservers;
        }

        // Proxy
        proxyconf = network.proxyConfig;
        if (proxyconf["Method"] !== proxy.state) {
            proxyconf["Method"] = proxy.state;
            if (proxy.state === "auto") {
                proxyconf["URL"] = proxyurl.text;
            } else if (proxy.state === "manual") {
                proxyconf["Servers"] = [proxyserver.text + ":" + proxyport.text];
            }
            network.proxyConfig = proxyconf;
        } else if (proxy.state === "manual") {
            proxy_server_text = proxyserver.text + ":" + proxyport.text;
            if (proxyconf["Servers"].length === 0 || proxyconf["Servers"][0] !== proxy_server_text) {
                proxyconf["Servers"] = [proxy_server_text];
                network.proxyConfig = proxyconf;
            }
        } else if (proxy.state == "auto") {
            if (proxyAutoUrl.checked && proxyconf["URL"]) {
                // empty URL to use the provided by DHCP
                proxyconf["URL"] = "";
                network.proxyConfig = proxyconf;
            } else if (! proxyAutoUrl.checked && proxyurl.text !== proxyconf["URL"]) {
                // manual URL is used and it needs update
                proxyconf["URL"] = proxyurl.text;
                network.proxyConfig = proxyconf;
            }
        }


    }

    content: Flickable {
        anchors.fill: parent
        anchors.leftMargin: 10
        anchors.topMargin: 10
        //contentWidth: mainColumn.width
        contentHeight: mainColumn.height
        flickableDirection: Flickable.VerticalFlick

        Column {
            id: mainColumn
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.right: parent.right
            spacing: 10

            Rectangle {
                anchors { left: parent.left; right: parent.right }
                height: 80
                color: "transparent"

                Image {
                    anchors { left: parent.left; verticalCenter: parent.verticalCenter }
                    source: "image://theme/icon-m-common-wlan-strength5"
                    width: 60
                    height: 60
                }

                Text {
                    anchors { left: parent.left; verticalCenter: parent.verticalCenter; leftMargin: 80 }
                    text: sheet.network ? sheet.network.name : "Error"
                    color: "white"
                    font.pointSize: 18
                }
            }

            Rectangle {
                anchors { left: parent.left; right: parent.right }
                height: 100
                color: "transparent"
                Button {
                    id: disconnectButton
                    anchors {
                        horizontalCenter: parent.horizontalCenter
                        verticalCenter: parent.verticalCenter
                    }
                    text: "Disconnect"
                    onClicked: {
                        console.log("Disconnect clicked");
                        network.requestDisconnect();
                        sheet.close();
                    }
                }
            }

            Item {
                anchors { left: parent.left; right: parent.right }
                height: 100
                Text {
                    anchors { left: parent.left; leftMargin: 20 }
                    text: "Method"
                    color: "grey"
                    font.pointSize: 14
                }
                ButtonRow {
                    id: method
                    anchors { left: parent.left; right: parent.right; top: parent.top; topMargin: 30; leftMargin: 10; rightMargin: 10 }
                    state: sheet.network ? sheet.network.ipv4["Method"] : "manual"

                    states: [
                        State {
                            name: "dhcp"
                            PropertyChanges {target: networkInfo; visible: true}
                            PropertyChanges {target: networkFields; visible: false}
                        },
                        State {
                            name: "manual"
                            PropertyChanges {target: networkInfo; visible: false}
                            PropertyChanges {target: networkFields; visible: true}
                        }
                    ]

                    Button {
                        text: "DHCP"
                        checked: sheet.network ? sheet.network.ipv4["Method"] == "dhcp" : false
                        onClicked: {
                            method.state = "dhcp"
                        }
                    }
                    Button {
                        text: "Static"
                        checked: sheet.network ? sheet.network.ipv4["Method"] == "manual" : true
                        onClicked: {
                            method.state = "manual"
                        }
                    }
                }
            }

            Item {
                id: networkInfo
                anchors { left: parent.left; right: parent.right }
                height: 340

                Column {
                    spacing: 50
                    Item {
                        height: 40
                        anchors { left: parent.left; right: parent.right }

                        Text {
                            anchors { left: parent.left; leftMargin: 20; top: parent.top }
                            text: "IP address"
                            color: "grey"
                            font.pointSize: 14
                        }
                        Text {
                            anchors { left: parent.left; leftMargin: 20; top:parent.top; topMargin: 30 }
                            text: sheet.network ? sheet.network.ipv4["Address"] : ""
                            color: "white"
                            font.pointSize: 20
                        }
                    }
                    Item {
                        height: 40
                        anchors { left: parent.left; right: parent.right }

                        Text {
                            anchors { left: parent.left; leftMargin: 20; top: parent.top }
                            text: "Subnet mask"
                            color: "grey"
                            font.pointSize: 14
                        }
                        Text {
                            anchors { left: parent.left; leftMargin: 20; top: parent.top; topMargin: 30 }
                            text: sheet.network ? sheet.network.ipv4["Netmask"] : ""
                            color: "white"
                            font.pointSize: 20
                        }
                    }
                    Item {
                        height: 40
                        anchors { left: parent.left; right: parent.right }

                        Text {
                            anchors { left: parent.left; leftMargin: 20; top: parent.top }
                            text: "Router"
                            color: "grey"
                            font.pointSize: 14
                        }
                        Text {
                            anchors { left: parent.left; leftMargin: 20; top: parent.top; topMargin: 30 }
                            text: sheet.network ? sheet.network.ipv4["Gateway"] : ""
                            color: "white"
                            font.pointSize: 20
                        }
                    }
                    Item {
                        height: 40
                        anchors { left: parent.left; right: parent.right }

                        Text {
                            anchors { left: parent.left; leftMargin: 20; top: parent.top }
                            text: "DNS"
                            color: "grey"
                            font.pointSize: 14
                        }
                        Text {
                            anchors { left: parent.left; leftMargin: 20; top: parent.top; topMargin: 30 }
                            text: sheet.network ? sheet.network.nameservers.join() : ""
                            color: "white"
                            font.pointSize: 20
                        }
                    }
                }
            }

            Item {
                id: networkFields
                anchors { left: parent.left; right: parent.right }
                height: 360

                Column {
                    spacing: 50
                    Item {
                        height: 40
                        anchors { left: parent.left; right: parent.right }

                        Text {
                            anchors { left: parent.left; leftMargin: 20; top: parent.top }
                            text: "IP address"
                            color: "grey"
                            font.pointSize: 14
                        }
                        TextField {
                            id: address
                            anchors { left: parent.left; leftMargin: 20; top:parent.top; topMargin: 30 }
                            width: 440
                            text: sheet.network ? sheet.network.ipv4["Address"] : ""
                            font.pointSize: 20
                        }
                    }
                    Item {
                        height: 40
                        anchors { left: parent.left; right: parent.right }

                        Text {
                            anchors { left: parent.left; leftMargin: 20; top: parent.top }
                            text: "Subnet mask"
                            color: "grey"
                            font.pointSize: 14
                        }
                        TextField {
                            id: netmask
                            anchors { left: parent.left; leftMargin: 20; top: parent.top; topMargin: 30 }
                            width: 440
                            text: sheet.network ? sheet.network.ipv4["Netmask"] : ""
                            font.pointSize: 20
                        }
                    }
                    Item {
                        height: 40
                        anchors { left: parent.left; right: parent.right }

                        Text {
                            anchors { left: parent.left; leftMargin: 20; top: parent.top }
                            text: "Router"
                            color: "grey"
                            font.pointSize: 14
                        }
                        TextField {
                            id: gateway
                            anchors { left: parent.left; leftMargin: 20; top: parent.top; topMargin: 30 }
                            width: 440
                            text: sheet.network ? sheet.network.ipv4["Gateway"] : ""
                            font.pointSize: 20
                        }
                    }
                    Item {
                        height: 40
                        anchors { left: parent.left; right: parent.right }

                        Text {
                            anchors { left: parent.left; leftMargin: 20; top: parent.top }
                            text: "DNS"
                            color: "grey"
                            font.pointSize: 14
                        }
                        TextField {
                            id: nameserversField
                            anchors { left: parent.left; leftMargin: 20; top: parent.top; topMargin: 30 }
                            width: 440
                            text: {
                                var nservs = "";
                                if (sheet.network) {
                                    nservs = sheet.network.nameserversConfig.join();
                                    return nservs ? nservs : sheet.network.nameservers.join();
                                } else {
                                    return "";
                                }
                            }
                            font.pointSize: 20
                        }
                    }
                }
            }

            Item {
                height: 100
                anchors { left: parent.left; right: parent.right }

                Text {
                    anchors { left: parent.left; leftMargin: 20; top: parent.top }
                    text: "Search domains"
                    color: "grey"
                    font.pointSize: 14
                }
                TextField {
                    id: domainsField
                    anchors { left: parent.left; leftMargin: 20; top: parent.top; topMargin: 30 }
                    width: 440
                    text: sheet.network ? sheet.network.domains.join() : ""
                    font.pointSize: 20
                }
            }

            Item {
                anchors { left: parent.left; right: parent.right }
                height: 100
                Text {
                    anchors { left: parent.left; leftMargin: 20 }
                    text: "HTTP Proxy"
                    color: "grey"
                    font.pointSize: 14
                }
                ButtonRow {
                    id: proxy
                    anchors { left: parent.left; right: parent.right; top: parent.top; topMargin: 30; leftMargin: 10; rightMargin: 10 }
                    state: sheet.network ? sheet.network.proxy["Method"] : "auto"

                    states: [
                        State {
                            name: "direct"
                            PropertyChanges {target: proxyManualFields; visible: false}
                            PropertyChanges {target: proxyAutoFields; visible: false}
                            PropertyChanges {target: directProxyButton; checked: true}
                            PropertyChanges {target: manualProxyButton; checked: false}
                            PropertyChanges {target: autoProxyButton; checked: false}
                        },
                        State {
                            name: "auto"
                            PropertyChanges {target: proxyManualFields; visible: false}
                            PropertyChanges {target: proxyAutoFields; visible: true}
                            PropertyChanges {target: directProxyButton; checked: false}
                            PropertyChanges {target: manualProxyButton; checked: false}
                            PropertyChanges {target: autoProxyButton; checked: true}
                        },
                        State {
                            name: "manual"
                            PropertyChanges {target: proxyManualFields; visible: true}
                            PropertyChanges {target: proxyAutoFields; visible: false}
                            PropertyChanges {target: directProxyButton; checked: false}
                            PropertyChanges {target: manualProxyButton; checked: true}
                            PropertyChanges {target: autoProxyButton; checked: false}
                        }
                    ]

                    Button {
                        id: directProxyButton
                        text: "Off"
                        onClicked: {
                            proxy.state = "direct"
                        }
                    }
                    Button {
                        id: manualProxyButton
                        text: "Manual"
                        onClicked: {
                            proxy.state = "manual"
                        }
                    }
                    Button {
                        id: autoProxyButton
                        text: "Auto"
                        onClicked: {
                            proxy.state = "auto"
                        }
                    }
                }
            }

            Item {
                id: proxyManualFields
                anchors { left: parent.left; right: parent.right }
                height: 440

                Column {
                    spacing: 50
                    Item {
                        height: 40
                        anchors { left: parent.left; right: parent.right }

                        Text {
                            anchors { left: parent.left; leftMargin: 20; top: parent.top }
                            text: "Server"
                            color: "grey"
                            font.pointSize: 14
                        }
                        TextField {
                            id: proxyserver
                            anchors { left: parent.left; leftMargin: 20; top:parent.top; topMargin: 30 }
                            width: 440
                            text: sheet.network && sheet.network.proxyConfig["Servers"] ? sheet.network.proxyConfig["Servers"][0].split(":")[0] : ""
                            font.pointSize: 20
                            // TODO: validator
                        }
                    }
                    Item {
                        height: 40
                        anchors { left: parent.left; right: parent.right }

                        Text {
                            anchors { left: parent.left; leftMargin: 20; top: parent.top }
                            text: "Port"
                            color: "grey"
                            font.pointSize: 14
                        }
                        TextField {
                            id: proxyport
                            anchors { left: parent.left; leftMargin: 20; top: parent.top; topMargin: 30 }
                            width: 440
                            text: sheet.network && sheet.network.proxyConfig["Servers"] ? sheet.network.proxyConfig["Servers"][0].split(":")[1] : ""
                            font.pointSize: 20
                            // TODO: validator
                        }
                    }
                }
            }

            Item {
                id: proxyAutoFields
                anchors { left: parent.left; right: parent.right }
                height: 440

                Column {
                    spacing: 50
                    CheckBox {
                        id: proxyAutoUrl
                        text: "Use URL provided by DHCP server"
                    }
                    Item {
                        height: 40
                        visible: !proxyAutoUrl.checked
                        anchors { left: parent.left; right: parent.right }

                        Text {
                            anchors { left: parent.left; leftMargin: 20; top: parent.top }
                            text: "URL"
                            color: "grey"
                            font.pointSize: 14
                        }
                        TextField {
                            id: proxyurl
                            anchors { left: parent.left; leftMargin: 20; top:parent.top; topMargin: 30 }
                            width: 440
                            readOnly: proxyAutoUrl.checked
                            text: {
                                if (sheet.network) {
                                    return sheet.network.proxy["URL"] ? sheet.network.proxy["URL"] : "";
                                } else {
                                    return "Error!";
                                }
                            }
                            font.pointSize: 20
                            // TODO: validator
                        }
                    }
                }
            }
        }
    }
}
