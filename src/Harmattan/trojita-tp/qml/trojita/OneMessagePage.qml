import QtQuick 1.1
import com.nokia.meego 1.0
import com.nokia.extras 1.1
import net.flaska.QNAMWebView 1.0
import "Utils.js" as Utils

Page {
    property string mailbox
    property alias url: messageView.url

    tools: commonTools

    Item {
        anchors {left: parent.left; right: parent.right; bottom: parent.bottom; top: header.bottom}

        Flickable {
            id: view
            anchors.fill: parent
            contentHeight: col.height
            contentWidth: col.width

            Column {
                id: col

                AddressWidget {
                    caption: qsTr("From")
                    address: imapAccess.oneMessageModel ? imapAccess.oneMessageModel.from[0] : undefined
                    width: view.width
                }
                AddressWidget {
                    caption: qsTr("To")
                    address: imapAccess.oneMessageModel ? imapAccess.oneMessageModel.to[0] : undefined
                    width: view.width
                }
                AddressWidget {
                    caption: qsTr("Cc")
                    address: imapAccess.oneMessageModel ? imapAccess.oneMessageModel.cc[0] : undefined
                    width: view.width
                }
                AddressWidget {
                    caption: qsTr("Bcc")
                    address: imapAccess.oneMessageModel ? imapAccess.oneMessageModel.bcc[0] : undefined
                    width: view.width
                }

                Label {
                    id: dateLabel
                    width: view.width
                    text: imapAccess.oneMessageModel ? qsTr("<b>Date:</b> ") + Utils.formatDate(imapAccess.oneMessageModel.date) : ""
                }

                Label {
                    id: subjectLabel
                    width: view.width
                    wrapMode: Text.Wrap
                    text: imapAccess.oneMessageModel ? qsTr("<b>Subject:</b> ") + imapAccess.oneMessageModel.subject : ""
                }

                QNAMWebView {
                    id: messageView
                    networkAccessManager: imapAccess.msgQNAM

                    preferredWidth: view.width
                    preferredHeight: view.height

                    // Without specifying the width here, plaintext e-mails would cause useless horizontal scrolling
                    width: parent.width

                    // Setting the URL from here would not be enough, we really want to force a reload whenever the message changes,
                    // even though the URL might remain the same

                    settings.userStyleSheetUrl: "data:text/css;charset=utf-8;base64," +
                                                Qt.btoa("* {color: white; background: black; font-size: " +
                                                        UiConstants.BodyTextFont.pixelSize + "px;};")
                }
            }
        }

        ScrollDecorator {
            flickableItem: view
        }
    }

    PageHeader {
        id: header
        text: imapAccess.oneMessageModel ? imapAccess.oneMessageModel.subject : ""
        anchors {left: parent.left; right: parent.right; top: parent.top}
    }
}
