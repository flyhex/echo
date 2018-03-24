#include "ProjectWnd.h"
#include <qfiledialog.h>
#include "Studio.h"
#include "Update.h"
#include <engine/core/io/archive/7zipArchive.h>
#include <engine/core/util/PathUtil.h>

namespace Studio
{
	// constructor
	ProjectWnd::ProjectWnd(QMainWindow* parent /* = 0 */)
		: QMainWindow(parent)
	{
		setupUi(this);

		// 隐藏标题
		setWindowFlags(windowFlags() | Qt::FramelessWindowHint);

		// 设置菜单左上控件
		m_menuBar->setTopLeftCornerIcon(":/icon/Icon/icon.png");

		m_previewerWidget = new QT_UI::QPreviewWidget(m_recentProject);
		m_previewerWidget->setContextMenuPolicy(Qt::CustomContextMenu);
		m_previewerWidget->isNeedFullPath(true);

		m_layout->addWidget(m_previewerWidget);

		QObject::connect(tabWidget, SIGNAL(currentChanged(int)), this, SLOT(openNewProject(int)));
		QObject::connect(m_previewerWidget, SIGNAL(Signal_onDoubleClickedItem(const QString&)), this, SLOT(onDoubleClicked(const QString&)));
		QObject::connect(m_previewerWidget, SIGNAL(Signal_onClickedItem(const QString&)), this, SLOT(onClicked(const QString&)));
		QObject::connect(m_versionListWidget, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(onDownloadNewVersion(QListWidgetItem*)));

		// 显示所有可更新版本
		showAllUpdateableVersion();
	}

	ProjectWnd::~ProjectWnd()
	{
		delete m_previewerWidget;
		m_previewerWidget = NULL;
	}

	void ProjectWnd::addRecentProject(const char* project)
	{
		if ( m_previewerWidget && NULL != project && 0 != project[0])
		{
			Echo::String icon = Echo::PathUtil::GetFileDirPath(project) + "icon.png";
			m_previewerWidget->addItem(project, icon.c_str());
		}
	}

	void ProjectWnd::openNewProject(int index)
	{
		if ( 2 == index )
		{
			QString projectName = QFileDialog::getOpenFileName(this, tr("Open Project"), "", tr("(*.echo)"));
			if ( !projectName.isEmpty() )
			{
				AStudio::Instance()->getMainWindow()->showMaximized();
				AStudio::Instance()->OpenProject(projectName.toStdString().c_str());
				AStudio::Instance()->getRenderWindow();

				close();
			}
			else
			{
				tabWidget->setCurrentIndex(0);
			}
		}

		// create project
		else if (1 == index)
		{
			QString projectName = QFileDialog::getSaveFileName(this, tr("New Project"), "", tr("(*.echo)"));
			if (!projectName.isEmpty())
			{
				// 0.confirm path and file name
				Echo::String fullPath = projectName.toStdString().c_str();
				Echo::String filePath = Echo::PathUtil::GetFileDirPath(fullPath);
				Echo::String fileName = Echo::PathUtil::GetPureFilename(fullPath, false);

				// 1.create directory
				Echo::String newFilePath = filePath + fileName + "/";
				if (!Echo::PathUtil::IsDirExist(newFilePath))
				{
					Echo::PathUtil::CreateDir(newFilePath);

					// 2.copy zip
					Echo::String writePath = newFilePath + "blank_project.7z";
					QFile qfile(":/Project/Project/blank_project.7z");
					if (qfile.open(QIODevice::ReadOnly))
					{
						// write files
						QFile writeFile(writePath.c_str());
						if (writeFile.open(QIODevice::WriteOnly))
						{
							writeFile.write(qfile.readAll());
							writeFile.close();
						}

						qfile.close();
					}

					// 3.unzip
					if (Echo::PathUtil::IsFileExist(writePath))
					{
						Echo::SzArchive::extractTo(writePath, newFilePath);

						Echo::PathUtil::DelPath(writePath);
					}

					// 4.rename
					Echo::String projectPathName = newFilePath + "blank.echo";
					Echo::String destProjectPathName = newFilePath + fileName + ".echo";
					if (Echo::PathUtil::IsFileExist(projectPathName))
						Echo::PathUtil::RenameFile(projectPathName, destProjectPathName);

					// 5.open project
					AStudio::Instance()->getMainWindow()->showMaximized();
					AStudio::Instance()->OpenProject(destProjectPathName.c_str());
					AStudio::Instance()->getRenderWindow();

					close();
				}
				else
				{
					EchoLogError("[%s] has existed", newFilePath.c_str());
				}
			}
			else
			{
				tabWidget->setCurrentIndex(0);
			}
		}
	}

	void ProjectWnd::onDoubleClicked(const QString& name)
	{
		AStudio::Instance()->getMainWindow()->showMaximized();
		AStudio::Instance()->OpenProject(name.toStdString().c_str());
		AStudio::Instance()->getRenderWindow();

		close();
	}

	void ProjectWnd::onClicked(const QString& name)
	{
		Echo::String msg = name.toStdString().c_str();

		m_statusBar->showMessage(name);
	}

	// 显示所有可更新版本
	void ProjectWnd::showAllUpdateableVersion()
	{
		// 检测更新
		Studio::Update updater;
		Echo::StringArray allVersions = updater.getAllEnabledVersions();
		for (Echo::String& version : allVersions)
		{
			QListWidgetItem* item = new QListWidgetItem(version.c_str());
			m_versionListWidget->addItem(item);
		}
	}

	// QTBUG-39220
	void ProjectWnd::showEvent(QShowEvent* event)
	{
		setAttribute(Qt::WA_Mapped);
		QMainWindow::showEvent(event);
	}

	// 点击下载
	void ProjectWnd::onDownloadNewVersion(QListWidgetItem* item)
	{
		Echo::String resName = item->text().toStdString().c_str();
		
		Studio::Update updater;
		updater.downloadVersion(resName);

		// 关闭当前程序
		close();
	}
}