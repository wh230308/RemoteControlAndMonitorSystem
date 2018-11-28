#ifndef UTILITY_H
#define UTILITY_H

class QLabel;
class QString;

class Utility
{
public:
    Utility();

    static void fillLabelWithImage(QLabel *label, int width, int height, const QString &imagePath);
};

#endif // UTILITY_H
