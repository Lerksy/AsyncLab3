#include <QCoreApplication>
#include <QVector>
#include <QRandomGenerator>
#include <QtAlgorithms>
#include <QtConcurrent>
#include <QFutureSynchronizer>

QVector<int> createArray(size_t size){
    QVector<int> array;
    for(size_t i = 0; i < size; i++){
        array.push_back(QRandomGenerator::global()->bounded(100, 8192));
    }
    return array;
}

int maxOfArray(QVector<int> array){
    qint64 max = array.at(0);
    for(auto i:array){
        if(i > max) max = i;
    }
    return max;
}

QVector<int> leaveMoreKMax(QVector<int> array){
    int border = static_cast<int>(maxOfArray(array)*0.7);
    for(size_t i = 0; i < static_cast<size_t>(array.size()); i++){
        if(array.at(i) <= border){
            array.removeAt(i);
            i--;
        }
    }
    return array;
}

QVector<int> leaveAliquotK(QVector<int> array){
    for(size_t i = 0; i < static_cast<size_t>(array.size()); i++){
        if(array.at(i)%3 != 0){
            array.removeAt(i);
            i--;
        }
    }
    return array;
}

QVector<int> sortOverload(QVector<int> array){
    std::sort(array.begin(), array.end());
    return array;
}


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QFutureSynchronizer<QVector<int>> sync; // Enabling sync machine.
    auto arrayOneFuture = QtConcurrent::run(createArray, 65536).then(leaveMoreKMax).then(sortOverload); // First array async run.
    auto arrayTwoFuture = QtConcurrent::run(createArray, 65536).then(leaveAliquotK).then(sortOverload); // Second array async run.
    sync.addFuture(arrayOneFuture); // Adding first QFuture to syncer.
    sync.addFuture(arrayTwoFuture); // Adding second QFuture to syncer.
    sync.waitForFinished(); // Awaiting for async jobs finished.
    QVector<int> resultVector = arrayOneFuture.result() + arrayTwoFuture.result(); // Merging arrays.
    std::sort(resultVector.begin(), resultVector.end()); // Sorting result array.
    qDebug()<<resultVector; // Output resulting array.

    return a.exec();
}
