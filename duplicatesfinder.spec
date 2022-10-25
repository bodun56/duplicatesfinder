%global	debug_package %{nil}

Summary:	Duplicates Finder
Name:		duplicatesfinder
Version:	1.6.6
Release:	1
License:	LGPL2.1
Group:		File tools
Url:		https://github.com/bodun56/duplicatesfinder
Source0:	https://github.com/bodun56/duplicatesfinder/archive/refs/tags/%{version}.tar.gz?/%{name}-%{version}.tar.gz

BuildRequires:	pkgconfig(Qt5Core)
BuildRequires:	pkgconfig(Qt5Gui)
BuildRequires:	pkgconfig(Qt5Sql)
BuildRequires:	pkgconfig(Qt5Widgets)

%description
Duplicates file finder.

%files
%{_bindir}/%{name}
%{_iconsdir}/hicolor/scalable/apps/%{name}.svg
%{_datadir}/applications/%{name}.desktop
#------------------------------------------------------------
%prep
%autosetup -p1

%build
%qmake_qt5
%make_build

%install
make install INSTALL_ROOT=%{buildroot}

# install icons
install -d %{buildroot}%{_iconsdir}/hicolor/scalable/apps
install -m0644 %{name}.svg %{buildroot}%{_iconsdir}/hicolor/scalable/apps/%{name}.svg

# install desktop file
install -d %{buildroot}%{_datadir}/applications
install -m0644 %{name}.desktop %{buildroot}%{_datadir}/applications/%{name}.desktop
