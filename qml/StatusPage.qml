import QtQuick 1.1
import com.nokia.meego 1.0
import com.nokia.controlpanel 0.1
import MeeGo.Connman 0.2

Page {
    id: statusPage
    tools: DcpToolBar {}

    property NetworkService network
    property alias networkLabel: networkNameLabel.text

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
                state: statusPage.network.ipv4["Method"]

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
                    checked: statusPage.network.ipv4["Method"] == "dhcp"
                    onClicked: {
                        method.state = "dhcp"
                    }
                }
                Button {
                    text: "Static"
                    checked: statusPage.network.ipv4["Method"] == "manual"
                    onClicked: {
                        method.state = "manual"
                    }
                }
            }
        }

        Item {
            id: networkInfo
            anchors { left: parent.left; right: parent.right }
            height: 200

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
                        text: statusPage.network.ipv4["Address"]
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
                        text: statusPage.network.ipv4["Netmask"]
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
                        text: statusPage.network.ipv4["Gateway"]
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
                        text: statusPage.network.nameservers.join()
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
                        text: statusPage.network.domains.join()
                        color: "white"
                        font.pointSize: 20
                    }
                }
            }
        }

        Item {
            id: networkFields
            anchors { left: parent.left; right: parent.right }
            height: 200
            visible: false

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
                        text: statusPage.network.ipv4["Address"]
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
                        text: statusPage.network.ipv4["Netmask"]
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
                        text: statusPage.network.ipv4["Gateway"]
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
                        text: statusPage.network.nameservers.join()
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
                        text: statusPage.network.domains.join()
                        font.pointSize: 20
                    }
                }
            }
        }
    }
}
