import QtQuick 1.1
import com.nokia.meego 1.0
import MeeGo.Connman 0.2
import com.nokia.controlpanel 0.1
import "/usr/lib/qt4/imports/Connman/js/mustache.js" as M

PageStackWindow {
    id: mainWindow
    initialPage: mainPage

    // this is a workaround for orientation bug triggered in case PageStackWindow
    // is used as a top level component in CP plugin
    rotation: screen.orientationString == 'Portrait' ? 90 : 0

    property variant netfields: {}

    function handleInput(key, value) {
        var dict = mainWindow.netfields;
        var isDoneEnabled = false;
        console.log("Received from TextField " + key + " " + value);
        dict[key] = value;
        mainWindow.netfields = dict;
        for (var id in mainWindow.netfields) {
            console.log(id + "-> " + mainWindow.netfields[id]);
            isDoneEnabled = isDoneEnabled || mainWindow.netfields[id].length;
        }
        doneButton.enabled = isDoneEnabled;
    }

    Timer {
        id: scanTimer
        interval: 25000
        running: networkingModel.wifiPowered
        repeat: true
        triggeredOnStart: true
        onTriggered: networkingModel.requestScan();
    }

    NetworkingModel {
        id: networkingModel
        property string form_tpl: "
            import QtQuick 1.1
            import com.nokia.meego 1.0
            Item {
                id: form
                anchors { fill: parent; margins: 10 }
                Column {
                    spacing: 5
                    anchors { fill: parent }
                    {{#fields}}
                    Text {
                        text: '{{name}}'
                        color: 'white'
                        font.pointSize: 14
                    }
                    TextField {
                        id: {{id}}
                        signal send (string key, string value)
                        anchors { left: parent.left; right: parent.right }
                        placeholderText: 'enter {{name}}'
                        Component.onCompleted: {
                            {{id}}.send.connect(handleInput);
                        }
                        onTextChanged: {
                            console.log('Sending from TextField {{id}}' + {{id}}.text);
                            {{id}}.send('{{name}}', {{id}}.text);
                        }
                    }
                    {{/fields}}
                }
            }
        "

        onTechnologiesChanged: {
            wifiSwitch.checked = networkingModel.wifiPowered;
            scanTimer.running = networkingModel.wifiPowered;
        }

        onWifiPoweredChanged: {
            wifiSwitch.checked = networkingModel.wifiPowered;
            scanTimer.running = networkingModel.wifiPowered;
        }

        onUserInputRequested: {
            scanTimer.running = false;
            scanTimer.triggeredOnStart = false;
            console.log("USER INPUT REQUESTED");
            var view = {
                "fields": []
            };
            for (var key in fields) {
                view.fields.push({
                    "name": key,
                    "id": key.toLowerCase(),
                    "type": fields[key]["Type"],
                    "requirement": fields[key]["Requirement"]
                });
                console.log(key + ":");
                for (var inkey in fields[key]) {
                    console.log("    " + inkey + ": " + fields[key][inkey]);
                }
            }
            var output = M.Mustache.render(form_tpl, view);
            console.log("output:" + output);
            var form = Qt.createQmlObject(output, dynFields, "dynamicForm1");
            if (pageStack.currentPage == networkPage) {
                console.log("Warning: already on networkPage");
            }
            if (pageStack.busy) {
                console.log("TODO: handle wrong input!!!");
            } else {
                pageStack.push(networkPage);
            }
        }

        onErrorReported: {
            console.log("Got error from model: " + error);
            if (error == "invalid-key") {
                mainpageNotificationBanner.text = "Incorrect value entered. Try again."
            } else {
                mainpageNotificationBanner.text = "Connect failed"
            }
            mainpageNotificationBanner.show()
        }
    }

    Component {
        id: networkRow

        Rectangle {
            height: 80
            color: "black"
            anchors { left: parent.left; right: parent.right }

            Row {

                Image {
                    source: {
                        var strength = modelData.strength;
                        var str_id = 0;

                        if (strength >= 59) {
                            str_id = 5;
                        } else if (strength >= 55) {
                            str_id = 4;
                        } else if (strength >= 50) {
                            str_id = 3;
                        } else if (strength >= 45) {
                            str_id = 2;
                        } else if (strength >= 30) {
                            str_id = 1;
                        }
                        return "image://theme/icon-m-common-wlan-strength" + str_id;
                    }
                    width: 60
                    height: 60
                }

                Column {
                    Text {
                        anchors { left: parent.left; leftMargin: 20 }
                        text: modelData.name ? modelData.name : "hidden network"
                        color: "white"
                        font.pointSize: 18
                    }
                    Text {
                        anchors { left: parent.left; leftMargin: 20 }
                        text: {
                            var state = modelData.state;
                            var security = modelData.security[0];

                            if ((state == "online") || (state == "ready")) {
                                return "connected";
                            } else if (state == "association" || state == "configuration") {
                                return "connecting...";
                            } else {
                                if (security == "none") {
                                    return "open";
                                } else {
                                    return "secure";
                                }
                            }
                        }
                        color: {
                            var state = modelData.state;
                            if (state == "online" || state == "ready") {
                                return "gold";
                            } else {
                                return "white";
                            }
                        }
                        font.pointSize: 14
                    }
                }
            }

            MouseArea {
                anchors.fill: parent
                onClicked: {
                    console.log("clicked " + modelData.name);
                    if (modelData.state == "idle" || modelData.state == "failure") {
                        modelData.requestConnect();
                        networkName.text = modelData.name;
                    } else {
                        console.log("Show network status page");
                        for (var key in modelData.ipv4) {
                            console.log(key + " -> " + modelData.ipv4[key]);
                        }
                        settingsSheet.network = modelData;
                        settingsSheet.open();
                    }
                }
            }
        }
    }

    Page {
        id: mainPage
        tools: DcpToolBar {}

        Column {
            spacing: 10
            anchors { fill: parent }
            Rectangle {
                anchors { left: parent.left; right: parent.right }
                height: 80
                color: "black"
                Text {
                    anchors { left: parent.left; verticalCenter: parent.verticalCenter; leftMargin: 20 }
                    text: "Wi-Fi networks"
                    color: "white"
                    font.pointSize: 18
                }
                Switch {
                    id: wifiSwitch
                    anchors { right: parent.right; verticalCenter: parent.verticalCenter; rightMargin: 40 }
                    checked: networkingModel.wifiPowered
                    onCheckedChanged: {
                        networkingModel.wifiPowered = wifiSwitch.checked
                    }
                }
            }

            ListView {
                anchors { left: parent.left; right: parent.right }
                clip: true
                height: 700
                spacing: 5
                model: networkingModel.networks
                delegate: networkRow
            }
        }
    }

    Page {
        id: networkPage

        Column {
            spacing: 10
            anchors { fill: parent }
            Rectangle {
                color: "black"
                anchors { left: parent.left; right: parent.right }
                height: 5
            }
            Rectangle {
                color: "#222222"
                anchors { left: parent.left; right: parent.right }
                height: 1
            }
            Rectangle {
                color: "black"
                anchors { left: parent.left; right: parent.right }
                height: 60
                Button {
                    anchors {
                        left: parent.left;
                        leftMargin: 20
                        verticalCenter: parent.verticalCenter
                    }
                    text: "Cancel"
                    width: 150
                    onClicked: {
                        networkingModel.sendUserReply({});
                        pageStack.pop()
                        scanTimer.running = true;
                    }
                }
                Button {
                    id: doneButton
                    anchors {
                        right: parent.right;
                        rightMargin: 20
                        verticalCenter: parent.verticalCenter
                    }
                    text: 'Done'
                    width: 150
                    enabled: false
                    platformStyle: ButtonStyle {
                        background: 'image://theme/meegotouch-button'+__invertedString+'-background-selected'+(position?'-'+position:'')
                    }
                    onClicked: {
                        console.log('clicked Done ' + 'x:' + x + ' y:' + y);
                        var fields = mainWindow.netfields;
                        for (var key in fields) {
                            console.log(key + " --> " + fields[key]);
                        }
                        pageStack.pop()
                        scanTimer.running = true;
                        networkingModel.sendUserReply(fields);
                    }
                }
            }
            Rectangle {
                color: "#333333"
                anchors { left: parent.left; right: parent.right }
                height: 1
            }
            Text {
                anchors { left: parent.left; leftMargin: 10 }
                text: "Sign in to secure Wi-Fi network"
                color: "white"
                font.pointSize: 18
            }
            Text {
                id: networkName
                anchors { left: parent.left; leftMargin: 10 }
                color: "white"
                font.pointSize: 18
            }
            Rectangle {
                color: "black"
                anchors { left: parent.left; right: parent.right }
                height: 30
            }
            Rectangle {
                anchors { left: parent.left; right: parent.right }
                height: 200
                color: "black"
                id: dynFields
            }
        }
    }

    SettingsSheet {
        id: settingsSheet
    }

    NotificationBanner {
        id: mainpageNotificationBanner
    }

}
