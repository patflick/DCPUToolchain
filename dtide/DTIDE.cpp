#include <cassert>
#include <QDebug>
#include "DTIDE.h"

DTIDE::DTIDE(Project* p, QWidget* parent): QMainWindow(parent)
{
    debuggingWindow = 0;
    menu = menuBar();

    tabs = new DTIDETabWidget(this);
    tabs->setMovable(true);
    setCentralWidget(tabs);

    debuggingSession = 0;
    glWidgets = new DTIDEGLWidgets();
    project = p;
    toolchain = project->getToolchain();
    toolchain->SetWidgetFactory(glWidgets);

    setWindowTitle("dtide - " + project->getTitle());

    setupMenuBar();
    setupActions();
    setupSignals();
    setupDockWidgets();

    resize(QSize(640, 580));

    QDir::setCurrent(project->getRootPath());
    
    QList<QString> files = project->getFileTabs();
    for(int i = 0; i < files.size(); i++) 
    {
        addCodeTab(files[i]);
    }
  
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(cycleUpdate()));
    timer->start(1);
}

void DTIDE::cycleUpdate()
{
    runCycles(100); // 1ms = 1kHz, 100 * 1kHz = 100kHz
}

void DTIDE::runCycles(int count)
{
    if (debuggingSession != 0)
    {
        if (count == 1)
            this->toolchain->Step();
        else
            for (int i = 0; i < count; i++)
                this->toolchain->Cycle();

        this->toolchain->SendStatus();
        this->toolchain->PostBatchHook();

        while (debuggingSession->HasMessages())
        {
            DebuggingMessage m = debuggingSession->GetMessage();
            switch (m.type)
            {
                case StatusType:
                {
                    StatusMessage status = (StatusMessage&) m.value;
                    debuggingWindow->setRegisters(status);
                    break;
                }
                case MemoryDumpType:
                {
                    MemoryDumpMessage memory = (MemoryDumpMessage&) m.value;
                    uint16_t* store = memory.data;
                    QByteArray* ram = new QByteArray();
                    for(int i = 0; i < 0x10000; i++)
                    {
                        ram->push_back(store[i] >> 8);
                        ram->push_back(store[i] & 0xff);
                    }

                    debuggingWindow->setMemoryData(ram);
                    break;
                }
                case MemoryType:
                {
                    MemoryMessage memory = (MemoryMessage&) m.value;
                    debuggingWindow->setMemoryAt(memory.pos, memory.value >> 8, memory.value & 0xff);
                    break;
                }
                case LineHitType:
                {
                    LineHitMessage hit = (LineHitMessage&) m.value;
                    Line line = hit.line;
                   
                    for (int i = 0; i < tabs->count(); i++)
                    {
                        CodeEditor* w = qobject_cast<CodeEditor*>(tabs->widget(i));
                        if(w->getFileName() == QString::fromLocal8Bit(line.Path))
                            w->highlightLine(line.LineNumber);
                    }
                    break;
                }
                default:
                    // Not implemented.
                    break;
            }
        }
    }
}


void DTIDE::addCodeTab(const QString& fileName)
{

    CodeEditor* editor = new CodeEditor(toolchain, fileName, this);
    connect(editor, SIGNAL(fileNameChanged(QString)), tabs, SLOT(updateTitle(QString)));

    tabs->addTab(editor, fileName);
}



void DTIDE::setupActions()
{
    nextTab = new QAction(tr("Next tab"), this);
    nextTab->setShortcut(QKeySequence::NextChild);
    connect(nextTab, SIGNAL(triggered()), tabs, SLOT(goToNextTab()));
    addAction(nextTab);
}

void DTIDE::setupSignals()
{
    connect(this, SIGNAL(fileSave()), tabs, SLOT(fileSave()));
    connect(glWidgets, SIGNAL(spawnGLWidget(QGLWidget*, QString, int, int)), this, SLOT(addGLWidget(QGLWidget*, QString, int, int)));
    connect(glWidgets, SIGNAL(killDockWidget(QGLWidget*)), this, SLOT(killDockWidget(QGLWidget*)));
}

void DTIDE::setupMenuBar()
{
    QMenu* file = new QMenu("&File", this);
    menu->addMenu(file);

    file->addAction("&New file", this, SLOT(newFile()), tr("Ctrl+N"));
    file->addAction("&Open file", this, SLOT(openFile()), tr("Ctrl+O"));
    file->addAction("&Save file", this, SLOT(saveFile()), tr("Ctrl+S"));

    QMenu* project = new QMenu("&Project", this);
    menu->addMenu(project);

    project->addAction("Compil&e", this, SLOT(compileProject()), tr("Ctrl+E"));
    project->addAction("Compile and &run", this, SLOT(compileAndRunProject()), tr("Ctrl+R"));
}

void DTIDE::showDebuggerWindow()
{
    debuggingWindow = new DTIDEDebuggingWindow(this);
    connect(debuggingWindow, SIGNAL(step()), this, SLOT(step()));
    connect(debuggingWindow, SIGNAL(resume()), this, SLOT(resume()));
    connect(debuggingWindow, SIGNAL(pause()), this, SLOT(pause()));
    connect(debuggingWindow, SIGNAL(stop()), this, SLOT(stop()));

    debuggingWindow->show();
}

void DTIDE::setupDockWidgets()
{
/*    QDockWidget* registersDockWidget = new QDockWidget(tr("Registers"), this);
    registersDockWidget->setAllowedAreas(Qt::LeftDockWidgetArea |
                                         Qt::RightDockWidgetArea);

    registersDockWidget->setWidget(registers);
    registersDockWidget->setMinimumWidth(100);
    addDockWidget(Qt::RightDockWidgetArea, registersDockWidget);*/

    QDockWidget* dirViewDockWidget = new QDockWidget(tr("File system"), this);
    dirViewDockWidget->setWidget(new DTIDEDirView(project->getRootPath()));
    addDockWidget(Qt::LeftDockWidgetArea, dirViewDockWidget); 
}

void DTIDE::addGLWidget(QGLWidget* w, QString title, int width, int height)
{
    QDockWidget* glDockWidget = new QDockWidget(title, this);
    glDockWidget->setWidget(w);
    glDockWidget->setMinimumWidth(width);
    glDockWidget->setMinimumHeight(height);
    glDockWidget->setFloating(true);
    addDockWidget(Qt::LeftDockWidgetArea, glDockWidget);

    dockWidgets.append(glDockWidget);
}

void DTIDE::step()
{
    runCycles(1);
}

void DTIDE::stop()
{
    if(debuggingWindow)
    {
        debuggingWindow = 0;
    }

    for (int i = 0; i < tabs->count(); i++)
    {
        CodeEditor* w = qobject_cast<CodeEditor*>(tabs->widget(i));
        w->stopHighlighting();
    }

    toolchain->Stop(debuggingSession);
}

void DTIDE::pause()
{
    toolchain->Pause(debuggingSession);
}

void DTIDE::resume()
{
    toolchain->Resume(debuggingSession);
}

void DTIDE::newFile()
{
    //    addCodeTab(DTIDEBackends::getUntitledProperties(type));
}

void DTIDE::openFile()
{
}

void DTIDE::saveFile()
{
    emit fileSave();
}

QSize DTIDE::sizeHint()
{
    return QSize(640, 480);
}

void DTIDE::compileAndRunProject()
{
    if(debuggingWindow)
        debuggingWindow->close();

    stop();
    debuggingSession = new DTIDEDebuggingSession();
    compileProject();

    showDebuggerWindow();

    QList<QString> compilableFiles = project->getCompilableFiles();
    for (int i = 0; i < tabs->count(); i++)
    {
        CodeEditor* w = qobject_cast<CodeEditor*>(tabs->widget(i));
        if(compilableFiles.contains(w->getFileName()))
        {
            qDebug() << "running " << w->getFileName();
            QList<Breakpoint> breakpoints(w->getBreakpoints());
            w->run(debuggingSession);
    
            if(!breakpoints.empty())
            {
                for(int i = 0; i < breakpoints.size(); i++)
                {
                    toolchain->AddBreakpoint(debuggingSession, breakpoints[i]);
                }
            }
        } 
    }
}

void DTIDE::compileProject()
{
    QList<QString> compilableFiles = project->getCompilableFiles();
    for (int i = 0; i < tabs->count(); i++)
    {
        CodeEditor* w = qobject_cast<CodeEditor*>(tabs->widget(i));
        if(compilableFiles.contains(w->getFileName()))
        {
            qDebug() << "compiling";
            w->ResetBuild();
            w->build();
        }
    }
}

void DTIDE::closeEvent(QCloseEvent* event)
{
    // clean up
}

void DTIDE::killDockWidget(QGLWidget* w)
{
    for (int i = 0; i < dockWidgets.size(); i++)
    {
        QDockWidget* dW = dockWidgets[i];
        if (dW->widget() == w)
        {
            removeDockWidget(dW);
            dockWidgets.removeAt(i);
            return;
        }
    }
}
