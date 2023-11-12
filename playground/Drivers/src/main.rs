extern crate bindgen;

use std::env;
use std::path::{Path, PathBuf};
use std::fs;

fn main() {
    // Find and collect all header files in the CWrapper folder
    let header_files = std::fs::read_dir("CWrapper")
        .expect("Failed to read CWrapper directory")
        .filter_map(|entry| {
            let entry = entry.expect("Failed to read directory entry");
            let path = entry.path();
            if path.is_file() && path.extension().map(|ext| ext == "h").unwrap_or(false) {
                Some(path)
            } else {
                None
            }
        })
        .collect::<Vec<_>>();

    // Output directory for the generated Rust files
    let out_dir = PathBuf::from("src/generated"); // Set the desired output directory

    // Create the output directory if it doesn't exist
    fs::create_dir_all(&out_dir).expect("Failed to create output directory");

    // Generate Rust bindings for each header file
    for header_file in &header_files {
        let header_path = header_file.to_str().expect("Failed to convert path to string");

        let bindings = bindgen::Builder::default()
            .header(header_path)
            .generate()
            .expect("Failed to generate bindings");

        // Save the bindings to a separate Rust file for each header
        let output_file = out_dir.join(format!("bindings_{}.rs", header_file.file_stem().unwrap().to_str().unwrap()));
        bindings.write_to_file(output_file).expect("Failed to write bindings file");
    }
}
