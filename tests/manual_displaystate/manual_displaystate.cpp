#include <QObject>
#include <qmdisplaystate.h>
#include <qmlocks.h>
#include <QTest>
#include <QDebug>
#include <iostream>

using namespace MeeGo;
using namespace std;

class TestClass : public QObject
{
    Q_OBJECT
public slots:
    void displayStateChanged(MeeGo::QmDisplayState::DisplayState state) {
        qDebug() << "Received a displayStateChanged signal: " << stateToString(state);
        currentState = state;
    }
private:
    QmDisplayState *displaystate;
    QmDisplayState::DisplayState currentState;
    MeeGo::QmLocks *locks;
    
private slots:
    void initTestCase() {
        displaystate = new MeeGo::QmDisplayState();
        QVERIFY(displaystate);
        locks = new MeeGo::QmLocks();
        QVERIFY(locks);
        QVERIFY(connect(displaystate, SIGNAL(displayStateChanged(MeeGo::QmDisplayState::DisplayState)),
                        this, SLOT(displayStateChanged(MeeGo::QmDisplayState::DisplayState))));
    }

    QString stateToString(QmDisplayState::DisplayState state) {
        QString retval;
        switch (state) {
        case QmDisplayState::Dimmed:
            retval = "Dimmed";
            break;
        case QmDisplayState::On:
            retval = "On";
            break;
        case QmDisplayState::Off:
            retval = "Off";
            break;
        case QmDisplayState::Unknown:
            retval = "Unknown";
            break;
        }
        return retval;
    }

    void stateChangeFunc(QmDisplayState::DisplayState state) {
        QString stateStr = stateToString(state);
        printf("Setting the display To %s...\n", stateStr.toAscii().data());
        QVERIFY2(displaystate->set(state), "Could not set the state!");

        QString question;
        if (state == QmDisplayState::Dimmed) {
            printf("Please look at the display, and see that it is dimmed for approximately 3 seconds and then Off\n");
            question = QString("Was the display dimmed and then Off (y/n)?");
            QTest::qWait(3000);
        } else {
            printf("Please look at the display, and verify that it is really %s\n", stateStr.toAscii().data());
            question = QString("Is the display %1 (y/n)?").arg(stateStr);
        }

        string res = "";
        do {
            //printf("\nIs the display %s? (y/n)?\n", stateStr.toAscii().data());
            printf("\n%s\n", question.toAscii().data());
            cin >> res;
        } while (res != "y" && res != "Y" && res != "n" && res != "N" && res != "\n");

        if (res == "n" || res == "N") {
            QFAIL("The display was not in the required state!");
        }
    }

    void testStates() {
        printf("This test will set the display to different states.\n");
        printf("You need to check that the display really is in the given state.\n\n");

        for (int i=-1; i <=1; i++) {
            stateChangeFunc((QmDisplayState::DisplayState)i);
        }
    }

    void testSetBlankingPause() {
        printf("This test will try to keep the display on for 3 minutes by calling setBlankingPause \n");
        printf("Please see that the display statays on...\n\n");

        if (MeeGo::QmLocks::Locked == locks->getState(MeeGo::QmLocks::TouchAndKeyboard)) {
            QVERIFY(locks->setState(MeeGo::QmLocks::TouchAndKeyboard, MeeGo::QmLocks::Unlocked));
            printf("Unlock touch screen and keyboard\n");
        }
        for (int i=0; i < 3; i++)
        {
            printf("%d second mark...\n", i*60);
            QVERIFY(displaystate->setBlankingPause());
            QTest::qWait(60*1000);
        }
        QTime timestamp = QTime::currentTime();
        string res = "";
        do {
            printf("Did the display stay on? (y/n)?\n");
            cin >> res;
        } while (res != "y" && res != "Y" && res != "n" && res != "N" && res != "\n");

        if (res == "n" || res == "N") {
            QFAIL("The display was not in the required state!");
        }

        int time = displaystate->getDisplayDimTimeout() + displaystate->getDisplayBlankTimeout() - timestamp.secsTo(QTime::currentTime());
        if (time > 0) {
            printf("The screen should start blanking again in %d...\n", time);
            QTest::qWait(time*1000);
        }
        printf("Display state: %d\n", displaystate->get());
        do {
            printf("Did the display blank again? (y/n)?\n");
            cin >> res;
        } while (res != "y" && res != "Y" && res != "n" && res != "N" && res != "\n");

        if (res == "n" || res == "N") {
            QFAIL("The display was not in the required state!");
        }
    }

    void testCancelBlankingPause() {
        printf("This test will call setBlankingPause() and then cancel it.\n");
        printf("Please see that the display blanks in the given time...\n");

        printf("Please wake the screen up by tapping it, and then press enter to start the test\n");
        std::cin.get();
        printf("Setting blanking pause...");
        displaystate->setBlankingPause();
        QTest::qWait(5*1000);
        printf("Canceling blanking pause...");
        displaystate->cancelBlankingPause();
        int time = displaystate->getDisplayBlankTimeout() + displaystate->getDisplayDimTimeout();
        printf("The display should blank again %d...\n", time);
        QTest::qWait(time*1000);
        string res = "";
        do {
            printf("Did the display blank again? (y/n)?\n");
            cin >> res;
        } while (res != "y" && res != "Y" && res != "n" && res != "N" && res != "\n");

        if (res == "n" || res == "N") {
            QFAIL("The display was not in the required state!");
        }
    }

    void cleanupTestCase() {
        delete displaystate;
        delete locks;
    }
};

QTEST_MAIN(TestClass)
#include "manual_displaystate.moc"
