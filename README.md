# imagisSRM

A desktop application for composing, managing, and transmitting **DICOM Structured Reports (SR)** in clinical radiology workflows.

Built with C++ and Qt5, imagisSRM lets radiologists and technologists load DICOM studies, author structured diagnostic reports (indication, procedure, findings, impression), and send them to remote PACS servers — all from a single interface.

---

## Features

- **DICOM Study Loading** — Parse DICOM files and extract patient, study, series, and instance metadata automatically.
- **Structured Report Authoring** — Compose clinical reports with standard sections: indication, procedure description, findings, and impression.
- **XML / DICOM SR Conversion** — Round-trip conversion between XML and DICOM SR format using DCMTK (`xml2dsr`, `dsr2xml`).
- **PACS Transmission** — Send completed reports to remote PACS servers over the DICOM network protocol via `dcmSend`.
- **Pending Queue** — Persist unsent reports to disk and retry transmission later without data loss.
- **Report Preview** — Generate HTML previews and DICOM thumbnail images for quick review before sending.
- **PDF Export** — Print or export reports to PDF via Qt's print support.
- **Background Threading** — Network send operations run asynchronously so the UI stays responsive.

---

## Tech Stack

| Component | Technology |
|---|---|
| Language | C++11 |
| GUI Framework | Qt 5 (Widgets, XML, PrintSupport, Concurrent) |
| DICOM Toolkit | [DCMTK](https://dicom.offis.de/dcmtk/) |
| Medical Imaging | [ITK](https://itk.org/) 5.x |
| Build System | CMake 3.15+ |

---

## Project Structure

```
imagisSRM/
├── CORE/                    # Business logic
│   ├── imgSRDataSetHandler  # DICOM metadata extraction
│   ├── imgSRCstore          # Format conversions, DICOM send, thumbnail generation
│   └── utils.h              # Shared constants and utility definitions
├── GUI/                     # User interface
│   ├── imgSRWidget          # Main application window
│   ├── imgSideWidgetElement # Collapsible sidebar report items
│   ├── imgXMLMaker          # XML generation from structured reports
│   ├── imgPendingFileHandler # Persistent queue for unsent reports
│   └── imgSendThread        # Async background thread for PACS transmission
├── AuxFolders/              # Sample DICOM files, pending queue, PDF outputs
├── Resources/               # Application icons
├── resources.qrc            # Qt resource bundle
└── CMakeLists.txt           # Build configuration
```

---

## Prerequisites

- CMake 3.15+
- Qt 5.x (Widgets, XML, PrintSupport, Concurrent)
- [DCMTK](https://dicom.offis.de/dcmtk/) — DICOM Toolkit
- [ITK](https://itk.org/) 5.x

> **Note:** `CMakeLists.txt` currently contains a hardcoded ITK path. Update the `ITK_DIR` variable to point to your local ITK build before compiling.

---

## Building

```bash
mkdir build && cd build
cmake ..
make
```

Run the application:

```bash
./imagisSRM
```

On startup the app loads a sample DICOM file from `AuxFolders/` for demonstration.

---

## DICOM Network Configuration

The application uses the following default DICOM network settings:

| Setting | Value |
|---|---|
| Calling AE Title | `DCMSEND` |
| Called AE Title | `ANY-SCP` |

These can be adjusted in the source before building to match your PACS configuration.

---

## License

This project is not yet licensed. Contact the author for usage terms.
