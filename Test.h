#include <QObject>
class Test : public QObject
{
    Q_OBJECT
public:
    Test(QObject *parent = nullptr);
public slots:
    int mime() {
        return 1;
    }
signals:
    void test();
};


