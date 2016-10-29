//
//  ViewController.swift
//  nRF8001-Swift
//
//  Created by Michael Teeuw on 31-07-14.
//  Copyright (c) 2014 Michael Teeuw. All rights reserved.
//

import UIKit
import PebbleKit

class ViewController: UIViewController, NRFManagerDelegate, PBPebbleCentralDelegate {
    
    var nrfManager:NRFManager!
    var feedbackView = UITextView()

    var pebbleCentral: PBPebbleCentral!
    var activeWatch: PBWatch?

    let pebbleAppUUID = UUID(uuidString: "a9931d36-1041-44a2-a8ba-e0e96cf114bc")!

    override func viewDidLoad() {
        super.viewDidLoad()
        
        nrfManager = NRFManager()
        
        nrfManager.verbose = true
        nrfManager.delegate = self
        
        setupUI()

        self.activeWatch = nil
        pebbleCentral = PBPebbleCentral.default()
        pebbleCentral.addAppUUID(pebbleAppUUID)
        pebbleCentral.delegate = self
    }

    func connectToWatch() {
        log("Connecting to watch...")
        pebbleCentral.run()
    }
}

// MARK: - NRFManagerDelegate Methods
extension ViewController
{
    func nrfDidConnect(_ nrfManager:NRFManager)
    {
        self.log("★ Connected to collar")
    }
    
    func nrfDidDisconnect(_ nrfManager:NRFManager)
    {
        self.log("★ Disconnected from collar")
    }
    
    func nrfReceivedData(_ nrfManager:NRFManager, data: Data?, string: String?) {
        self.log("↓ Received collar data - String: \(string) - Data: \(data)")
    }
}

// MARK: - PBPebbleCentralDelegate methods
extension ViewController {
    @objc func pebbleCentral(_ central: PBPebbleCentral, watchDidConnect watch: PBWatch, isNew: Bool) {
        log("watchDidConnect: \(watch.name)")
        guard activeWatch == nil else {
            return
        }
        activeWatch = watch

        DispatchQueue.main.async {
            watch.appMessagesAddReceiveUpdateHandler(self.handleAppMessage, with: self.pebbleAppUUID)
            self.log("subscribed")
        }

        // The AppMessage channel must be opened by the phone
        sendAppMessage(message: [:], withUUID: self.pebbleAppUUID)
    }

    @objc func pebbleCentral(_ central: PBPebbleCentral, watchDidDisconnect watch: PBWatch) {
        log("watchDidDisconnect: \(watch.name)")
        guard activeWatch == watch else {
            return
        }
        activeWatch = nil
    }

    func sendAppMessage(message: [NSNumber : AnyObject], withUUID uuid: UUID) {
        guard let watch = activeWatch else {
            log("Can't send message: no Pebble watch connected")
            return
        }

        log("sending \(message)")

        DispatchQueue.main.async {
            watch.appMessagesPushUpdate(message, with: uuid)
        }
    }

    func handleAppMessage(watch: PBWatch, message: [NSNumber : Any]) -> Bool {
        log("Received watch message: \(message)")

        if let string = message[0] as? String {
            let result = self.nrfManager.writeString(string)
            self.log("↑ Sent string to collar: \(string) - Result: \(result)")
        }

        return true
    }
}

// MARK: - Various stuff
extension ViewController {
    func setupUI()
    {
        view.addSubview(feedbackView)
        feedbackView.translatesAutoresizingMaskIntoConstraints = false
        feedbackView.layer.borderWidth = 1
        feedbackView.isEditable = false
        
        let connectButton:UIButton =  UIButton(type: .system)
        view.addSubview(connectButton)
        connectButton.setTitle("Connect to watch", for: UIControlState())
        connectButton.translatesAutoresizingMaskIntoConstraints = false
        connectButton.addTarget(self, action: #selector(ViewController.connectToWatch), for: UIControlEvents.touchUpInside)

        view.addConstraints(NSLayoutConstraint.constraints(withVisualFormat: "H:|-[f]-|", options: [], metrics: nil, views: ["f":feedbackView]))
        view.addConstraints(NSLayoutConstraint.constraints(withVisualFormat: "H:|-[c]-|", options: [], metrics: nil, views: ["c":connectButton]))
        view.addConstraints(NSLayoutConstraint.constraints(withVisualFormat: "V:|-20-[f]-[c]-20-|", options: [], metrics: nil, views: ["f":feedbackView,"c":connectButton]))
    }
    
    func log(_ string:String)
    {
        print(string)
        feedbackView.text = feedbackView.text + "\(string)\n"
        feedbackView.scrollRangeToVisible(NSMakeRange(feedbackView.text.characters.count , 1))
    }
    
}

