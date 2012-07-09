import QtQuick 1.1
import com.nokia.meego 1.0
import com.nokia.controlpanel 0.1
import MeeGo.Connman 0.2

Page {
    id: statusPage
    tools: DcpToolBar {}

    property NetworkService network
    property alias networkLabel: networkNameLabel.text

    Flickable {
        anchors { fill: parent }
        contentHeight: 1100

        Column {
            spacing: 10
            anchors { fill: parent }
            Item {
                anchors { left: parent.left; right: parent.right }
                height: 80

                Image {
                    anchors { left: parent.left; verticalCenter: parent.verticalCenter }
                    source: "image://theme/icon-m-common-wlan-strength5"
                    width: 60
                    height: 60
                }

                Text {
                    id: networkNameLabel
                    anchors { left: parent.left; verticalCenter: parent.verticalCenter; leftMargin: 80 }
                    text: "Network name"
                    color: "white"
                    font.pointSize: 18
                }
            }

            Item {
                anchors { left: parent.left; right: parent.right }
                height: 100
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
                        pageStack.pop();
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
                    state: statusPage.network ? statusPage.network.ipv4["Method"] : "manual"

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
                        checked: statusPage.network ? statusPage.network.ipv4["Method"] == "dhcp" : false
                        onClicked: {
                            method.state = "dhcp"
                        }
                    }
                    Button {
                        text: "Static"
                        checked: statusPage.network ? statusPage.network.ipv4["Method"] == "manual" : true
                        onClicked: {
                            method.state = "manual"
                        }
                    }
                }
            }

            Item {
                id: networkInfo
                anchors { left: parent.left; right: parent.right }
                height: 440

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
                            text: statusPage.network ? statusPage.network.ipv4["Address"] : ""
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
                            text: statusPage.network ? statusPage.network.ipv4["Netmask"] : ""
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
                            text: statusPage.network ? statusPage.network.ipv4["Gateway"] : ""
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
                            text: statusPage.network ? statusPage.network.nameservers.join() : ""
                            color: "white"
                            font.pointSize: 20
                        }
                    }
                    Item {
                        height: 60
                        anchors { left: parent.left; right: parent.right }

                        Text {
                            anchors { left: parent.left; leftMargin: 20; top: parent.top }
                            text: "Search domains"
                            color: "grey"
                            font.pointSize: 14
                        }
                        Text {
                            anchors { left: parent.left; leftMargin: 20; top: parent.top; topMargin: 30 }
                            text: statusPage.network ? statusPage.network.domains.join() : ""
                            color: "white"
                            font.pointSize: 20
                        }
                    }
                }
            }

            Item {
                id: networkFields
                anchors { left: parent.left; right: parent.right }
                height: 440

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
                            anchors { left: parent.left; leftMargin: 20; top:parent.top; topMargin: 30 }
                            width: 440
                            text: statusPage.network ? statusPage.network.ipv4["Address"] : ""
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
                            anchors { left: parent.left; leftMargin: 20; top: parent.top; topMargin: 30 }
                            width: 440
                            text: statusPage.network ? statusPage.network.ipv4["Netmask"] : ""
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
                            anchors { left: parent.left; leftMargin: 20; top: parent.top; topMargin: 30 }
                            width: 440
                            text: statusPage.network ? statusPage.network.ipv4["Gateway"] : ""
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
                            anchors { left: parent.left; leftMargin: 20; top: parent.top; topMargin: 30 }
                            width: 440
                            text: statusPage.network ? statusPage.network.nameservers.join() : ""
                            font.pointSize: 20
                        }
                    }
                    Item {
                        height: 60
                        anchors { left: parent.left; right: parent.right }

                        Text {
                            anchors { left: parent.left; leftMargin: 20; top: parent.top }
                            text: "Search domains"
                            color: "grey"
                            font.pointSize: 14
                        }
                        TextField {
                            anchors { left: parent.left; leftMargin: 20; top: parent.top; topMargin: 30 }
                            width: 440
                            text: statusPage.network ? statusPage.network.domains.join() : ""
                            font.pointSize: 20
                        }
                    }
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
                    state: statusPage.network ? statusPage.network.proxy["Method"] : "auto"

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
                            anchors { left: parent.left; leftMargin: 20; top:parent.top; topMargin: 30 }
                            width: 440
                            text: "this is Server"
                            font.pointSize: 20
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
                            anchors { left: parent.left; leftMargin: 20; top: parent.top; topMargin: 30 }
                            width: 440
                            text: "this is Port"
                            font.pointSize: 20
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
                    Item {
                        height: 40
                        anchors { left: parent.left; right: parent.right }

                        Text {
                            anchors { left: parent.left; leftMargin: 20; top: parent.top }
                            text: "URL"
                            color: "grey"
                            font.pointSize: 14
                        }
                        TextField {
                            anchors { left: parent.left; leftMargin: 20; top:parent.top; topMargin: 30 }
                            width: 440
                            text: statusPage.network ? statusPage.network.proxy["URL"] : "Error!"
                            font.pointSize: 20
                        }
                    }
                }
            }
        }
    }
}
